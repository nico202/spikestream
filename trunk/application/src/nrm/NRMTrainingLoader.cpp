
//SpikeStream includes
#include "NRMTrainingLoader.h"
#include "NRMException.h"
#include "NRMConstants.h"

//Other includes
#include <iostream>
#include <sstream>
using namespace std;

/*! Constructor */
NRMTrainingLoader::NRMTrainingLoader(NRMNetwork* network){
	this->network = network;
}


/*! Destructor */
NRMTrainingLoader::~NRMTrainingLoader(){
}


/*! Loads up the training from the specified file */
void NRMTrainingLoader::loadTraining(const char* filePath){
	//Open the file
	FILE* file = fopen(filePath, "rb");
	if ( !file ) {
	    ostringstream errMsg;
	    errMsg<<"Unable to open training file '"<<filePath<<"'";
	    throw NRMException(errMsg.str());
	}

	//Reset count of bytes we have read
	fileByteCount = 0;

	//Get the number of layers in the network
	int numNetworkLayers = network->getNeuralLayerCount();

	//Array listing the trained layers that need to be loaded
	int* layerIdArray;
	int numTrainedLayers = 0;

	//Check information in header is correct and extract the ids of layers to be read
	loadTrainingHeader(file, numNetworkLayers, numTrainedLayers, layerIdArray);

	//Load up the training data from the file
	for ( int n=0; n < numTrainedLayers; n++ ) {
		loadLayerTraining(layerIdArray[n], file);
	}

	fclose(file);
	delete [] layerIdArray;

	/* Need to reconstruct the random connections, which are not stored in any of the files, but
	   built from the same random seed each time. */
	network->createConnections();
}


/*! Reads in header of training file to check that it has the correct information */
void NRMTrainingLoader::loadTrainingHeader(FILE* file, int numNetworkLayers, int& numTrainedLayers, int*& layerIdArray){
	unsigned short val;

	//Read in version of file
	char buf[10];
	fReadFile(buf, 9, 1, file);
	buf[9] = '\0';

	//Check file is correct version
	if(strcmp(buf, "macconN03") != 0){
		fclose(file);
		throw NRMException("File has been saved with an unsupported version of NRM");
	}

	//Check that file contains correct number of layers
	fReadFile(&val, 2, 1, file);
	numTrainedLayers = val;
	if ( numTrainedLayers > numNetworkLayers ) {
		fclose(file);
		throw NRMException("Training data in file is for more layers than are present in the network");
	}

	//Read in the ids of the trained layers
	layerIdArray = new int[numTrainedLayers];
	for ( int n = 0; n < numTrainedLayers; n++ ) {
		fReadFile(&val, 2, 1, file);
		layerIdArray[n] = val;
	}
}


/*! Loads up the training data from the specified file */
void NRMTrainingLoader::loadLayerTraining(int layerId, FILE* file){
	//cout<<"Loading layer training, layer id="<<layerId<<endl;

	//Declare variables used for loading
	short val;
	unsigned short uval;
	short sval;
	unsigned int tmpHeight, tmpWidth;

	//Get the layer whose training we are trying to load
	NRMNeuralLayer* neuralLayer = network->getNeuralLayerById(layerId);

	//Identify the file version
	char buf[10];
	fReadFile (buf, 9, 1, file);
	buf[9] = '\0';

	if ( strcmp(buf, "macconL05") == 0) {
		loadVersion = 4;
	}
	else {
		fclose(file);
		throw NRMException("Layer version is not supported");
	}

	//Get the layer ID that we are going to load and check it matches the one that we have been requested to load.
	fReadFile(&val, 2, 1, file);
	if ( val != layerId ) {
		fclose(file);
		throw NRMException("Training data in file is for a different layer");
	}

	//Check that saved layer has correct number of colour planes
	fReadFile(&uval, 2, 1, file);
	if ( uval != neuralLayer->colPlanes ) {
		ostringstream errMsg;
		errMsg<<"WARNING: Colour planes in current layer do not match those in saved layer. ";
		errMsg<<"Current colour planes="<<neuralLayer->colPlanes<<" training colour planes="<<uval;
		//fclose(file);
		//throw NRMException(errMsg.str());
		cout<<"FIXME FIXME: "<<errMsg.str()<<endl;
	}

	//Check width and height match
	uval=22;
	fReadFile(&uval, 2, 1, file);
	tmpWidth = uval;
	fReadFile(&uval, 2, 1, file);
	tmpHeight = uval;
	if(tmpWidth != neuralLayer->width || tmpHeight != neuralLayer->height){
		fclose(file);
		ostringstream errMsg;
		errMsg<<"WARNING: Layer Training data in file does not match the width and/or height of the current layer. ";
		errMsg<<"Current: width="<<neuralLayer->width<<", height="<<neuralLayer->height<<"; training: width="<<tmpWidth<<", height="<<tmpHeight;
		throw NRMException(errMsg.str());
	}

	//Check connection paths
	QList<NRMConnection*> layerConns = neuralLayer->getConnections();
	fReadFile(&val, 2, 1, file);
	if ( val != layerConns.size() ) {
			fclose(file);
			throw NRMException("Number of connection paths in training data and connection do not match");
	}

	//Check that the training connection paths match those of the network connection paths
	for (int n = 0; n < layerConns.size(); n++ ) {
		fReadFile(&val, 2, 1, file);
		if ( val != layerConns[n]->destLayerId ) {
			fclose(file);
			throw NRMException("Connection paths in training and network do not match");
		}
	}

	//Check the destination type of the connection paths are ok
	for (int n = 0; n < layerConns.size(); n++ ) {
		fReadFile(&val, 2, 1, file);
		if ( val != layerConns[n]->destObjectType ) {
			if ( val == 5 && layerConns[n]->destObjectType == 55 )
				continue;
			if ( val == 55 && layerConns[n]->destObjectType == 5 )//Added the opposite case in to make it work
				continue;

			//Error if we have reached this point
			fclose(file);
			ostringstream errMsg;
			errMsg<<"Layer "<<layerId<<", Connection path "<<n<<". Training data in file is for connections to layer "<<layerConns[n]->destLayerId;
			switch ( val ) {
			case MAGNUS_PREV_OB:
				errMsg<<" MAGNUS Previous object";
				break;
			case MAGNUS_STATE_OB:
				errMsg<<" MAGNUS State object";
				break;
			case IMG_INPUT_OB: case 55:
				errMsg<<" Input object";
				break;
			}
			throw NRMException(errMsg.str());
		}
	}

	//Check that the destination colour planes match
	for (int n = 0; n < layerConns.size(); n++ ) {
		fReadFile(&val, 2, 1, file);

		//Check that colour plane in training data matches that in the destination layer
		NRMLayer* destLayer = network->getLayerById(layerConns[n]->destLayerId, layerConns[n]->destObjectType);
		if ( val != destLayer->colPlanes ) {
			ostringstream errMsg;
			errMsg<<"FIXME FIXME: Layer "<<layerId<<" Connection path "<<n<<". Training data in file is for destination object with "<<val<<" colour plane(s)";
			//fclose(file);
			//throw NRMException(errMsg.str());
		}
	}

	//Check that width and height match that of the destination layer
	for (int n = 0; n < layerConns.size(); n++ ) {
		//Read in width and height of the destination layer
		fReadFile(&val, 2, 1, file);
		tmpWidth = val;
		fReadFile(&val, 2, 1, file);
		tmpHeight = val;

		//Check width and height match that in destination layer
		NRMLayer* destLayer = network->getLayerById(layerConns[n]->destLayerId, layerConns[n]->destObjectType);
		if ( tmpWidth != destLayer->width || tmpHeight != destLayer->height){
			fclose(file);
			throw NRMException("Width and height of destination layer do not match that stored in training data");
		}
	}

	//Check that number of connections in the parameters is correct */
	for (int n = 0; n < layerConns.size(); n++ ) {
		fReadFile(&val, 2, 1, file);
		if ( val != layerConns[n]->conParams.numCons ) {
			fclose(file);
			ostringstream errMsg;
			errMsg<<"Layer "<<layerId<<" Connection path "<<n<<". Training data in file is for "<<val<<" connections per neuron";
			throw NRMException(errMsg.str());
		}
	}

	//Check that the sWidth and sHeight parameters are ok
	unsigned int lcWidth, lcHeight;
	for (int n = 0; n < layerConns.size(); n++ ) {
		fReadFile(&sval, 2, 1, file);
		lcWidth = sval;
		fReadFile(&sval, 2, 1, file);
		lcHeight = sval;
		if ( lcWidth != layerConns[n]->conParams.sWidth || lcHeight != layerConns[n]->conParams.sHeight ){
			fclose(file);
			ostringstream errMsg;
			errMsg<<"Layer "<<layerId<<" Connection path "<<n<<" Training data in file is for destination localization size "<<lcWidth<<"x"<<lcHeight;
			throw NRMException(errMsg.str());
		}
	}

	//Check that the sLeft and sTop parameters are ok
	unsigned int lcLeft, lcTop;
	for (int n = 0; n < layerConns.size(); n++ ) {
		fReadFile(&sval, 2, 1, file);
		lcLeft = sval;
		fReadFile(&sval, 2, 1, file);
		lcTop = sval;
		if ( layerConns[n]->conParams.gazeMap){//These parameters are only meaningful when it is a gaze map
			if(lcLeft != layerConns[n]->conParams.sLeft || lcTop != layerConns[n]->conParams.sTop ){
				fclose(file);
				ostringstream errMsg;
				errMsg<<"Training sLeft and sTop do not match values in current configuration. ";
				errMsg<<"Training values: sLeft="<<lcLeft<<"; sTop="<<lcTop<<". ";
				errMsg<<"Current values: sLeft="<<layerConns[n]->conParams.sLeft<<"; sTop="<<layerConns[n]->conParams.sTop;
				throw NRMException(errMsg.str());
			}
		}
	}

	//Check spreading value is correct - FIXME: DOES NOT MATCH AT PRESENT
	fReadFile(&val, 2, 1, file);
	if ( val != neuralLayer->spreading ) {
		ostringstream errMsg;
		errMsg<<"WARNING: Spreading value in training data does not match that in current configuration. ";
		errMsg<<"Training spreading="<<val<<"; current spreading="<<neuralLayer->spreading;
		cout<<errMsg.str()<<endl;
		//fclose(file);
		//throw NRMException (errMsg.str());//DOES NOT MATCH, IGNORE
	}

	//Read in the trained strings
	short numTrainingStrings;
	fReadFile(&numTrainingStrings, 2, 1, file); // num trained strings

	unsigned int patArrSize = neuralLayer->getPatternArraySize();
	cout<<"Num trained strings = "<<numTrainingStrings<<"; patArrSize="<<patArrSize<<" layer size="<<neuralLayer->getSize()<<". fileByteCount="<<fileByteCount<<endl;

	//Create neurons if they have not been created already - FIXME: THIS IS A BIT RUBBISH, BUT WORKS FOR THE MOMENT
	neuralLayer->createNeurons();

	unsigned int trainingStrArrSize = patArrSize + 1;
	unsigned char* trainingStrArray = new unsigned char [trainingStrArrSize];
	//NOTE: Assume that netType is SMALL_NET or BIG_NET
	for (int neurNum = 0; neurNum < neuralLayer->getSize(); neurNum++ ) {
		for (int i = 0; i < numTrainingStrings; i++ ) {
			fReadFile(trainingStrArray, trainingStrArrSize, 1, file);
			neuralLayer->getNeuron(neurNum)->addTraining(&trainingStrArray[1], patArrSize, trainingStrArray[0]);
		}
		//neuralLayer->printNeuronConnections(neurNum);
		//neuralLayer->getNeuron(neurNum)->printTraining();
	}


	//Clean up
	delete [] trainingStrArray;
}


/*! Wrapper for fread that does error checking and throws an exception if an unexpected number of bytes is read */
void NRMTrainingLoader::fReadFile(void* dataStruct, size_t sizeOfElement, size_t numElements, FILE* file ){
		size_t result = fread(dataStruct, sizeOfElement, numElements, file);
		if(result != numElements){
			//cout<<"Actual number of elements read="<<result<<"; expected number of elements = "<<numElements<<endl;
			if( feof(file) ){
				ostringstream errMsg;
				errMsg<<"Unexpected end of file. Tried to read "<<sizeOfElement * numElements<<" elements. ";
				errMsg<<"Read "<<result<<" elements. ";
				errMsg<<"ByteCount="<<fileByteCount;
				throw NRMException (errMsg.str());
			}
			else if( ferror(file) ){
				throw NRMException ("ferror encountered reading from file", fileByteCount);
			}
			else{
				throw NRMException ("Unknown error encountered when reading from file. ByteCount=", fileByteCount);
			}
		}
		else{
			fileByteCount += (sizeOfElement * numElements);
		}
}


void NRMTrainingLoader::printTrainingPattern(unsigned char* patternArray, unsigned int arraySize, int trainingStrNumber){
	unsigned char bitComparator;
	cout<<"------------------ Training String "<<trainingStrNumber<<"-------------------"<<endl;
	cout<<"Desired output ="<<(unsigned int) patternArray[0]<<endl;
	for(unsigned int byteCounter=1; byteCounter<arraySize; ++byteCounter){
		bitComparator = 128;//Should be 10000000
		for(int i=0; i<8; ++i){//Work through the bits in the byte
			if(bitComparator & patternArray[byteCounter])
				cout<<"1 ";
			else
				cout<<"0 ";
			bitComparator >>= 1;//shift one bit to the right
		}
		cout<<endl;
	}
}












