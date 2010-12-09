//SpikeStream includes
#include "NRMConfigLoader.h"
#include "NRMException.h"
#include "NRMConstants.h"
using namespace spikestream;

//Other includes
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;


/*! Constructor */
NRMConfigLoader::NRMConfigLoader(void){
	//Set network to reset state
	network = NULL;
}


/*! Destructor */
NRMConfigLoader::~NRMConfigLoader(void){
	//Clean up all the data structures
	reset();
}

/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Loads up the input layers, neural layers and connections from an NRM configuration file.
	This file should have the extension .cfg.  */
void NRMConfigLoader::loadConfig(const char* filePath){
	//Reset data structures holding information
	reset();

	//Create a new empty network
	network = new NRMNetwork();

	//Local variables to load from the file
	short val;
	int val2;

	//Open the file
	FILE* file = fopen(filePath, "rb");
	if ( !file ) {
		ostringstream errStr;
		errStr<<"Unable to open file '"<<filePath<<"'";
		throw NRMException(errStr.str());
	}

	//Read in the version of the file that we are loading
	loadConfigVersion(file);

	//This loader only supports config versions greater than or equal to 26
	if(network->getConfigVersion() != 26){
		fclose(file);
		throw NRMException("Files with configuration version less than 26 are not supported by this loader");
	}

	/* Window configuration information.
		Leave the read statements to keep place in file, ignore the rest */
	fReadFile(&val, 2, 1, file);
	if ( val ) {
		//TRect recx;
		//((MDIChildren*) childBase)->FrameRect(recx);
		//int x = recx.Left() + 2;
		//int y = recx.Top() + 2;
		fReadFile(&val, 2, 1, file);
		//int left = val - x;
		fReadFile(&val, 2, 1, file);
		//int top = val - y;
		fReadFile(&val, 2, 1, file);
		//int right = val - x;
		fReadFile(&val, 2, 1, file);
		//int bottom = val - y;
		//TRect rect(left,top,right,bottom);
		//((MDIChildren*) childBase)->CmNewInputTextViewer();
		//((MDIChildren*) childBase)->SetWindow(rect);
	}

	//Load up information about inputs
	loadInputs(file);

	//Load up information about neural layers
	loadNeuralLayers(file);

	/* Loading black and white and colour information
		- don't think this is needed by me */
	//bwAscii* bw = new bwAscii;
	fReadFile(&val2, 4, 1, file);
	//wsprintf(bw->width, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(bw->height, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(bw->black, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(bw->white, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(bw->start, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(bw->num, "%d", val2);

	//((MDIChildren*) base)->SetbwASCII(bw);
	//delete bw;

	//colAscii* col = new colAscii;
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->width, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->height, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->black, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->red, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->green, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->yellow, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->blue, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->magenta, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->cyan, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->white, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->start, "%d", val2);
	fReadFile(&val2, 4, 1, file);
	//wsprintf(col->num, "%d", val2);

	//((MDIChildren*) base)->SetcolASCII(col);
	//delete col;

	//Read the recall delay
	fReadFile(&val, 2, 1, file);
	//((MDIChildren*) base)->SetRecallDelay(val);

	//Load up information about the connections
	loadConnections(file);

	//Build the connections using the random number generator
	network->createConnections();
	//network->printConnections();

	//------------------------------------------------------------------
	//------------             LOAD DATA SETS                -----------
	//------------------------------------------------------------------
/*	int count = fread(&val, 2, 1, file);
	int sets = val;
	int num;
	bool loadFail = false;

	if ( count && val ) {
		char* fileName = new char [FILE_TITLE];
		for ( int n = 0; n < sets; n++ ) {
			fread(&val, 2, 1, file); // str len
			fread(fileName, val, 1, file);
			fileName[val] = '\0';
			//if ( LoadDataSet(fileName) )
		//	loadFail = true;
		}
		delete [] fileName;

		if ( loadFail ) {
			fclose(file);
			config = false;
			if ( bmpFail )  {
				if ( welcome ){
			welcome = 4;
				}
				throw NRMException("Not all bitmap files, recorded in the configuration\nfor loading into the Framed Input(s), were found\nin the local folder.\n\nAlso\n\nNot all Data Sets recorded in configuration were found\nin the local folder.\n\nNo Data Sets have been associated with panel objects.");
			}
			else {
				if ( welcome ){
				welcome = 3;
				}
			throw NRMException("Not all Data Sets recorded in configuration were found\nin the local folder.\n\nNo Data Sets have been associated with panel objects.");
			}
		}
		else {
		fread(&val, 2, 1, file);
		num = val;

		for ( int n = 0; n < num; n++ ) {
			fread(&val, 2, 1, file);
			if ( val > -1 )
				;//AutoAssociateInputSet(n, val);
		}

		fread(&val, 2, 1, file);
		num = val;

		for ( int n = 0; n < num; n++ ) {
			fread(&val, 2, 1, file);
			if ( val > -1 )
					;//AutoAssociateStateSet(n, val);
			fread(&val, 2, 1, file);
			if ( val > -1 )
					;//AutoAssociatePreviousSet(n, val);
		}
		}
	}

	//Throw exception if we cannot load bitmap files.
	if ( bmpFail ) {
		fclose(file);
		config = false;
		if ( welcome ){
		welcome = 2;
		}
		throw NRMException("Not all bitmap files, recorded in the configuration\nfor loading into the Framed Input(s), were found\nin the local folder");
	}
*/

	//Finish off
	fclose(file);
}


/*! Returns the network tha has been loaded */
NRMNetwork* NRMConfigLoader::getNetwork(){
	return network;
}


/*! Resets and cleans up the data structures associated with this class */
void NRMConfigLoader::reset(){
	fileByteCount = 0;
	if(network != NULL){
		delete network;
		network = NULL;
	}
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Wrapper for fread that does error checking and throws an exception if an unexpected number of bytes is read */
void NRMConfigLoader::fReadFile(void* dataStruct, size_t sizeOfElement, size_t numElements, FILE* file ){
	size_t result = fread(dataStruct, sizeOfElement, numElements, file);
	if(result != numElements){
		cout<<"Actual number of elements read="<<result<<"; expected number of elements = "<<numElements<<endl;
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


/*! Identifies the config number of the file */
void NRMConfigLoader::loadConfigVersion(FILE* file){
	//Variables used for loading
	char buf[10];

	//Read the version of NRM used to create the file
	fReadFile(buf, 9, 1, file);
	buf[9] = '\0';
	if ( !strcmp(buf, "macconC26") )
		network->setConfigVersion(26);
	else if ( !strcmp(buf, "macconC25") )
		network->setConfigVersion(25);
	else if ( !strcmp(buf, "macconC24") )
		network->setConfigVersion(24);
	else if ( !strcmp(buf, "macconC23") )
		network->setConfigVersion(23);
	else if ( !strcmp(buf, "macconC22") )
		network->setConfigVersion(22);
	else if ( !strcmp(buf, "macconC21") )
		network->setConfigVersion(21);
	else if ( !strcmp(buf, "macconC20") )
		network->setConfigVersion(20);
	else if ( !strcmp(buf, "macconC19") )
		network->setConfigVersion(19);
	else if ( !strcmp(buf, "macconC18") )
		network->setConfigVersion(18);
	else if ( !strcmp(buf, "macconC17") )
		network->setConfigVersion(17);
	else if ( !strcmp(buf, "macconC16") )
		network->setConfigVersion(16);
	else if ( !strcmp(buf, "macconC15") )
		network->setConfigVersion(15);
	else if ( !strcmp(buf, "macconC14") )
		network->setConfigVersion(14);
	else if ( !strcmp(buf, "macconC13") )
		network->setConfigVersion(13);
	else if ( !strcmp(buf, "macconC12") )
		network->setConfigVersion(12);
	else if ( !strcmp(buf, "macconC11") )
		network->setConfigVersion(11);
	else if ( !strcmp(buf, "macconC10") )
		network->setConfigVersion(10);
	else if ( !strcmp(buf, "macconC09") )
		network->setConfigVersion(9);
	else if ( !strcmp(buf, "macconC08") )
		network->setConfigVersion(8);
	else if ( !strcmp(buf, "macconC07") )
		network->setConfigVersion(7);
	else if ( !strcmp(buf, "macconC06") )
		network->setConfigVersion(6);
	else if ( !strcmp(buf, "macconC05") )
		network->setConfigVersion(5);
	else if ( !strcmp(buf, "macconC04") )
		network->setConfigVersion(4);
	else if ( !strcmp(buf, "macconC03") )
		network->setConfigVersion(3);
	else if ( !strcmp(buf, "macconC02") )
		network->setConfigVersion(2);
	else {
		buf[7] = '\0';
		if ( !strcmp(buf, "macconC") ) {
			fclose(file);
			throw NRMException("Configuration file produced\nwith higher upgrade module");
		}
		else {
			fclose(file);
			throw NRMException("Not a valid configuration file");
		}
	}

	//cout<<"Config version: "<<network->getConfigVersion()<<endl;
}


/*! Loads up information about the inputs */
void NRMConfigLoader::loadInputs(FILE* file){
	//Temporary loading variables
	int tmpId, frameSpecial, ndrWinNum = -1;
	short numInputWins, val, frameNum, oldFrameNum;
	unsigned short uval;
	char* tmpName;
	string oldFrameName;
	bool newFrame;
	unsigned char bVal, b2Val;

	fReadFile(&numInputWins, 2, 1, file);
	for (int n = 0; n < numInputWins; n++) {
		tmpId = n;//ID of the input layer

		NRMInputLayer* inputLayer = new NRMInputLayer();

		//Identify the type of input layer
		fReadFile(&val, 2, 1, file);
		inputLayer->winType = val;

		//Read information about input layer differently depending on the type.
		switch ( val ) {
			case FRAME_WIN: case _FRAME_WIN://Single or double frame within an image
				fReadFile(&val, 2, 1, file);
				inputLayer->width = val;
				fReadFile(&val, 2, 1, file);
				inputLayer->height = val;
				fReadFile(&val, 2, 1, file);
				inputLayer->colPlanes = val;
				if ( inputLayer->winType == _FRAME_WIN ) {//Double input frame
					fReadFile(&val, 2, 1, file);
					inputLayer->width2 = val;
					fReadFile(&val, 2, 1, file);
					inputLayer->height2 = val;
				}
				//rec.Set(0, 0, width + 2, height + 2);
				//SetCopyAtt(width, height, colPlanes, rec, width2, height2);
				if ( inputLayer->winType == FRAME_WIN ){
					;//((MDIChildren*) base)->CmNewFrameInput();
				}
				else  {
					;//((MDIChildren*) base)->CmNewDualFrameInput();
				}
				fReadFile(&val, 2, 1, file);
				inputLayer->x = val - 2;
				fReadFile(&val, 2, 1, file);
				inputLayer->y = val - 2;
				fReadFile(&val, 2, 1, file);
				inputLayer->r = val - 2;
				fReadFile(&val, 2, 1, file);
				inputLayer->b = val - 2;
				//rec.Set(x, y, r, b);
				//SetWindowRect(n, rec);

				//Large number of config specific conditionals
				if ( network->getConfigVersion() > 2 ) {
					fReadFile(&val, 2, 1, file);
					//SetInputMagControl(n, val);
				}
				if ( network->getConfigVersion() == 11 ) {
					fReadFile(&val, 2, 1, file);
					//SetFrameBrightConv(n, val, false);
				}
				if ( network->getConfigVersion() > 11 ) {
					fReadFile(&val, 2, 1, file);
					fReadFile(&uval, 2, 1, file);
				   //SetFrameBrightConv(n, val, uval);
				}
				if ( network->getConfigVersion() > 12 ) {
					fReadFile(&bVal, 1, 1, file);
					fReadFile(&b2Val, 1, 1, file);
					//SetFrameColMasks(n, bVal, b2Val);
//					if ( winType == FRAME_WIN ){
//						;//RandomizeFramedInput(n);
//					}
				}
				//Read in name of framed image
				if ( network->getConfigVersion() > 16 ) {
					fReadFile(&val, 2, 1, file);
					tmpName = new char [val + 1];
					for (int i = 0; i < val; i++ ) {
						fReadFile(&bVal, 1, 1, file);
						tmpName[i] = bVal;
					}
					tmpName[val] = '\0';
					inputLayer->frameName = tmpName;
					//cout<<"FRAME NAME LOAD: "<<inputLayer->frameName<<endl;
					//LoadFramedInputTitleName(n, name);
					delete [] tmpName;
				}
				if ( network->getConfigVersion() > 4 ) {
					fReadFile(&val, 2, 1, file);
					if ( val ) {
						tmpName = new char [val + 1];
						for (int i = 0; i < val; i++ ) {
							fReadFile(&bVal, 1, 1, file);
							tmpName[i] = bVal;
						}
						tmpName[val] = '\0';
						//if ( LoadInputBitmap(n, name) )
						//	bmpFail = true;
						delete [] tmpName;
					}
				}
				if ( network->getConfigVersion() > 9 ) {
					fReadFile(&val, 2, 1, file);
					if ( val ) {
						fclose(file);
						//((MDIChildren*) base)->DestConfig();
						throw NRMException("\nAttempt to load Gaze connectivity\nno longer supported.\n\nUse an earlier version of NRM.");
					}
					//SetGazePosDspInput(n, val);
				}
				if ( network->getConfigVersion() > 14 ) {
					//int posWin, width, height;
					fReadFile(&val, 2, 1, file);
					//posWin = val;
					fReadFile(&val, 2, 1, file);
					//width = val;
					fReadFile(&val, 2, 1, file);
					//height = val;
					//SetFramePosParams(n, posWin, width, height);
				}
				if ( network->getConfigVersion() > 18 ) {
					//int noDisp; int colThresh; bool invHor; bool invVer; int segBorder;
					fReadFile(&val, 2, 1, file);
					inputLayer->noDisp = val;
					fReadFile(&val, 2, 1, file);
					inputLayer->colThresh = val;
					fReadFile(&val, 2, 1, file);
					inputLayer->invHor = val;
					fReadFile(&val, 2, 1, file);
					inputLayer->invVer = val;
					if ( network->getConfigVersion() > 23 ) {
						fReadFile(&val, 2, 1, file);
						inputLayer->segBorder = val;
					}
					if ( inputLayer->noDisp )
						frameSpecial++;
					//((MDIChildren*) childBase)->SetSpecial(frameSpecial);
					//SetFrameInterpParams(n, noDisp, colThresh, invHor, invVer, segBorder, frameSpecial);
					if ( inputLayer->noDisp )
						ndrWinNum = n;
				   //SetFrameSegPos(n);
				}
			break;
			case MULTI_WIN: //Input held within an input panel
				fReadFile(&val, 2, 1, file);
				inputLayer->width = val;
				fReadFile(&val, 2, 1, file);
				inputLayer->height = val;
				fReadFile(&val, 2, 1, file);
				inputLayer->colPlanes = val;

				fReadFile(&val, 2, 1, file);
				inputLayer->x = val;
				fReadFile(&val, 2, 1, file);
				inputLayer->y = val;
				//rec.SetWH(0, 0, x, y);

				//SetCopyAtt(width, height, colPlanes, rec, 0);

				fReadFile(&val, 2, 1, file);
				if ( val == FRAME_REQ ) {
					//((MDIChildren*) base)->CmNewMultipleInputs();
					fReadFile(&frameNum, 2, 1, file);
					fReadFile(&val, 2, 1, file);
					inputLayer->x = val - 2;
					fReadFile(&val, 2, 1, file);
					inputLayer->y = val - 2;
					//SetInputFrameRect(frameNum, x, y);
				}
				else{
					fReadFile(&frameNum, 2, 1, file);
					//NewMultipleInputs(frameNum);
				}

				inputLayer->frameNum = frameNum;

				if ( frameNum != oldFrameNum ) {
					oldFrameNum = frameNum;
					newFrame = true;
				}
				else
					newFrame = false;


				//Config number specific conditionals
				//Read in the name of the frame the input is in
				if ( newFrame && network->getConfigVersion() > 16 ) {//Starting to load new frame
					fReadFile(&val, 2, 1, file);
					//SetInputFrameDir(frameNum, val);

					fReadFile(&val, 2, 1, file);
					tmpName = new char [val + 1];
					for (int i = 0; i < val; i++ ) {
						fReadFile(&bVal, 1, 1, file);
						tmpName[i] = bVal;
					}
					tmpName[val] = '\0';
					oldFrameName = tmpName;
					inputLayer->frameName = tmpName;
					//cout<<"NEW FRAME NAME LOAD: "<<inputLayer->frameName<<endl;
					//inputLayer->name = name;
					//LoadPanelInputTitleName(frameNum, name);
					delete [] tmpName;
				}
				else if(!newFrame && network->getConfigVersion() > 16){//Input is in an existing frame
					//cout<<"INPUT IN EXISTING FRAME: "<<oldFrameName<<"; ID="<<(n+1)<<endl;
					inputLayer->frameName = oldFrameName;
				}

				if ( network->getConfigVersion() == 11 ) {
					fReadFile(&val, 2, 1, file);
					//SetInputBrightConv(n, val);
				}
				if ( network->getConfigVersion() > 11 ) {
					fReadFile(&val, 2, 1, file);
					//SetInputBrightConv(n, val);
					fReadFile(&val, 2, 1, file);
					//SetInputGrey(n, val);
				}
				if ( network->getConfigVersion() > 12 ) {
					fReadFile(&bVal, 1, 1, file);
					//SetFixedColMask(n, bVal);
					if ( bVal != 255 ){
						;//RandomizeInput(n);
					}
				}
				//cout<<"Multi window: width="<<width<<"; height="<<height<<endl;
			break;
		}

		//Add input layer to network
		network->addInputLayer(tmpId, inputLayer);
	}

	//Print out information about input windows.
	//network->printInputLayers();

	if ( ndrWinNum > -1 ){
	   ;//RefreshNdWindows(ndrWinNum);
	}
}


/*! Loads up information about the neural layers */
void NRMConfigLoader::loadNeuralLayers(FILE* file){
	//Variables needed for loading
	short val, numNeuralWins, oldFrameNum = -11;
	int tmpId;
	bool newFrame;
	unsigned char bVal;
	char* tmpName;

	//Read the number of neural windows
	fReadFile(&numNeuralWins, 2, 1, file);
	if ( numNeuralWins > 3 && getOpVer() < 2 ) {
		fclose(file);
		//((MDIChildren*) base)->DestConfig();
		throw NRMException("Attempt to create more than 3 MAGNUS layers.\n\nThis feature is only available with Version 2.\n\nTo upgrade install NRM.upg\nfor Version 2 or higher.");
	}

	//Load the neural windows
	for ( int n = 0; n < numNeuralWins; n++ ) {
		tmpId = n;//Id of the neural layer

		//Create neural layer to be populated with the loaded information
		NRMNeuralLayer* neuralLayer = new NRMNeuralLayer();

		fReadFile(&val, 2, 1, file);
		switch ( val ) {
			case MAGNUS_WIN:
				fReadFile(&val, 2, 1, file);
				neuralLayer->width = val;
				fReadFile(&val, 2, 1, file);
				neuralLayer->height = val;
				fReadFile(&val, 2, 1, file);
				neuralLayer->colPlanes = val;
				fReadFile(&val, 2, 1, file);
				neuralLayer->x = val;
				fReadFile(&val, 2, 1, file);
				neuralLayer->y = val;
				//rec.SetWH(0, 0, x, y);
				fReadFile(&val, 2, 1, file);
				neuralLayer->inTrack = val;

				//SetCopyAtt(width, height, colPlanes, rec, inTrack);

				fReadFile(&val, 2, 1, file);
				neuralLayer->neuralType = val;

				fReadFile(&val, 2, 1, file);
				if ( val == FRAME_REQ ) {
					if ( neuralLayer->neuralType == MAGNUS_WIN_PREV ){
						;//((MDIChildren*) base)->CmNewMAGNUSFrame();
					}
					else{
						;//((MDIChildren*) base)->CmNewMAGNUSFrameNP();
					}
					fReadFile(&val, 2, 1, file);
					neuralLayer->frameNum = val;
					fReadFile(&val, 2, 1, file);
					neuralLayer->x = val - 2;
					fReadFile(&val, 2, 1, file);
					neuralLayer->y = val - 2;
					//SetNeuralFrameRect(frameNum, x, y);
				}
				else{
					fReadFile(&val, 2, 1, file);
					neuralLayer->frameNum = val;
					if ( neuralLayer->neuralType == MAGNUS_WIN_PREV ){
						;//NewMAGNUS(frameNum, true);
					}
					else{
						;//NewMAGNUS(frameNum, false);
					}
				}

				if ( neuralLayer->frameNum != oldFrameNum ) {
					oldFrameNum = neuralLayer->frameNum;
					newFrame = true;
				}
				else
					newFrame = false;


				if ( newFrame && network->getConfigVersion() > 16 ) {
					fReadFile(&val, 2, 1, file);
					//SetNeuralFrameDir(frameNum, val);

					fReadFile(&val, 2, 1, file);
					tmpName = new char [val + 1];
					for (int i = 0; i < val; i++ ) {
						fReadFile(&bVal, 1, 1, file);
						tmpName[i] = bVal;
					}
					tmpName[val] = '\0';
					neuralLayer->frameName = tmpName;
					//cout<<"FRAME NAME = "<<neuralLayer->frameName<<endl;
					//LoadPanelNeuralTitleName(frameNum, name);
					delete [] tmpName;
				}


				//Load in assorted parameters of the neural layer.
				fReadFile(&val, 2, 1, file);
				neuralLayer->generalisation = val;//Store generalization parameter
				//SetGeneralisation(n, val);
				fReadFile(&val, 2, 1, file);
				neuralLayer->spreading = val;//Store spreading parameter
				//SetSpreading(n, val);
				fReadFile(&val, 2, 1, file);
				//SetGenBound(n, val);
				neuralLayer->altParam1 = val;//Store alt parameter 1

				//Configuration specific conditionals
				if ( network->getConfigVersion() > 15 ) {
					fReadFile(&val, 2, 1, file);
					//SetGenBound2(n, val);
					neuralLayer->altParam2 = val;//Store alt parameter 2
				}
				if ( network->getConfigVersion() > 2 ) {
					fReadFile(&val, 2, 1, file);
					//MacCon->SetControlledInput(n, val);
					fReadFile(&val, 2, 1, file);
					//MacCon->SetConvCycles(n, val);
					fReadFile(&val, 2, 1, file);
				}
				if ( network->getConfigVersion() > 11 ) {
					fReadFile(&val, 2, 1, file);
					//SetNeuralBrightConv(n, val);
					fReadFile(&val, 2, 1, file);
					//SetNeuralGrey(n, val);
				}
				if ( network->getConfigVersion() > 12 ) {
					unsigned char bPVal;
					fReadFile(&bVal, 1, 1, file);
					fReadFile(&bPVal, 1, 1, file);
					//SetNeuralColMasks(n, bVal, bPVal);
				}
			break;
		}

		//Add neural layer to network
		network->addNeuralLayer(tmpId, neuralLayer);
	}

	//Print out the details of the neural layers
	//network->printNeuralLayers();
}


/*! Loads up information about the connections between layers */
void NRMConfigLoader::loadConnections(FILE* file){
	//Temporary variables used for loading
	short val, numCons;
	unsigned short uval;
	int DefConsWidth, InConsWidth, NeuPrevConsWidth, NeuStateConsWidth;
	int *HorizConsWidth, *ObHeight, InConnections,	NeuInConnections;
	int NeuPrevConnections,	NeuStateConnections, NumConnectSchemes, CurMaxCons;
	int InCurMaxCons, NeuPrevCurMaxCons, NeuStateCurMaxCons, *HorizCurMaxCons;
	int *CurMaxUpCons, CanvWidth, CanvHeight;

	fReadFile(&val, 2, 1, file);
	if ( val ) {
		//((MDIChildren*) base)->CmConnectionsChart();
		int numNeuralLayers = network->getNeuralLayerCount();
		int numInputLayers =network->getInputLayerCount();

		if ( numNeuralLayers >= numInputLayers ) {
			HorizConsWidth = new int[numNeuralLayers];
			ObHeight = new int[numNeuralLayers];
			HorizCurMaxCons = new int[numNeuralLayers];
			CurMaxUpCons = new int[numNeuralLayers];
		}
		else {
			HorizConsWidth = new int[numInputLayers];
			ObHeight = new int[numInputLayers];
			HorizCurMaxCons = new int[numInputLayers];
			CurMaxUpCons = new int[numInputLayers];
		}

		//Read in connection variables
		fReadFile(&val, 2, 1, file);
		DefConsWidth = val;
		fReadFile(&val, 2, 1, file);
		InConsWidth = val;
		fReadFile(&val, 2, 1, file);
		NeuPrevConsWidth = val;
		fReadFile(&val, 2, 1, file);
		NeuStateConsWidth = val;
		fReadFile(&val, 2, 1, file);
		InConnections = val;
		fReadFile(&val, 2, 1, file);
		NeuInConnections = val;
		fReadFile(&val, 2, 1, file);
		NeuPrevConnections = val;
		fReadFile(&val, 2, 1, file);
		NeuStateConnections = val;
		fReadFile(&val, 2, 1, file);
		NumConnectSchemes = val;
		fReadFile(&val, 2, 1, file);
		CurMaxCons = val;
		fReadFile(&val, 2, 1, file);
		InCurMaxCons = val;
		fReadFile(&val, 2, 1, file);
		NeuPrevCurMaxCons = val;
		fReadFile(&val, 2, 1, file);
		NeuStateCurMaxCons = val;
		fReadFile(&val, 2, 1, file);
		CanvWidth = val;
		fReadFile(&val, 2, 1, file);
		CanvHeight = val;

		if ( numNeuralLayers >= numInputLayers )
			for ( int i = 0; i < numNeuralLayers; i++) {
				fReadFile(&val, 2, 1, file);
				HorizConsWidth[i] = val;
				fReadFile(&val, 2, 1, file);
				ObHeight[i] = val;
				fReadFile(&val, 2, 1, file);
				HorizCurMaxCons[i] = val;
				fReadFile(&val, 2, 1, file);
				CurMaxUpCons[i] = val;
			}
		else {
			for ( int i = 0; i < numInputLayers; i++) {
				fReadFile(&val, 2, 1, file);
				HorizConsWidth[i] = val;
				fReadFile(&val, 2, 1, file);
				ObHeight[i] = val;
				fReadFile(&val, 2, 1, file);
				HorizCurMaxCons[i] = val;
				fReadFile(&val, 2, 1, file);
				CurMaxUpCons[i] = val;
			}
		}

		/*((MDIChildren*) base)->SetChartBaseVars(DefConsWidth,
										InConsWidth,
										NeuPrevConsWidth,
										NeuStateConsWidth,
										HorizConsWidth,
										ObHeight,
										InConnections,
										NeuInConnections,
										NeuPrevConnections,
										NeuStateConnections,
										NumConnectSchemes,
										CurMaxCons,
										InCurMaxCons,
										NeuPrevCurMaxCons,
										NeuStateCurMaxCons,
										HorizCurMaxCons,
										CurMaxUpCons,
										CanvWidth,
										CanvHeight, numNeuralWins, numInputWins );*/

		delete [] HorizConsWidth;
		delete [] ObHeight;
		delete [] HorizCurMaxCons;
		delete [] CurMaxUpCons;

		//cons* con;
		fReadFile(&numCons, 2, 1, file);
		//cout<<"Number of connections "<<numCons<<endl;
		for ( int n = 0; n < numCons; n++ ) {
			NRMConnection* con = new NRMConnection(network);
			fReadFile(&uval, 2, 1, file);
			con->x1 = uval;
			fReadFile(&uval, 2, 1, file);
			con->y1 = uval;
			fReadFile(&uval, 2, 1, file);
			con->x2 = uval;
			fReadFile(&uval, 2, 1, file);
			con->y2 = uval;
			fReadFile(&uval, 2, 1, file);
			con->x3 = uval;
			fReadFile(&uval, 2, 1, file);
			con->y3 = uval;
			fReadFile(&uval, 2, 1, file);
			con->x4 = uval;
			fReadFile(&uval, 2, 1, file);
			con->y4 = uval;
			fReadFile(&uval, 2, 1, file);
			con->x5 = uval;
			fReadFile(&uval, 2, 1, file);
			con->y5 = uval;
			fReadFile(&uval, 2, 1, file);
			con->x6 = uval;
			fReadFile(&uval, 2, 1, file);
			con->y6 = uval;
			fReadFile(&uval, 2, 1, file);
			con->selected = uval;
			fReadFile(&uval, 2, 1, file);
			con->srcLayerId = uval;
			fReadFile(&uval, 2, 1, file);
			con->srcObjectType = uval;
			fReadFile(&uval, 2, 1, file);
			con->destLayerId = uval;
			fReadFile(&uval, 2, 1, file);
			con->destObject = uval;
			fReadFile(&uval, 2, 1, file);
			con->destObjectType = uval;
			fReadFile(&uval, 2, 1, file);
			con->connection = uval;

			loadConnectionParameters(con->conParams, file);

			//((MDIChildren*) base)->AddCon(con);

			//Add connection to the appropriate neural layer
			network->getNeuralLayerById(con->srcLayerId)->addConnectionPath(con);

		}//End of for loop working through connections
		//((MDIChildren*) base)->ChartSetupWindow();

		short chartChildren;
		//int Selected, Connections, LayerConnections, MoreNeuLayers;
		fReadFile(&chartChildren, 2, 1, file);

	/*	for ( int n = 0; n < chartChildren; n++ ) {
			fread(&uval, 2, 1, file);
			Selected = uval;
			fread(&uval, 2, 1, file);
			Connections = uval;
			fread(&uval, 2, 1, file);
			LayerConnections = uval;
			fread(&uval, 2, 1, file);
			MoreNeuLayers = uval;
			if ( network->getConfigVersion() > 20 ){
				;//((MDIChildren*) base)->ChartVarSetup(n, Selected, Connections, LayerConnections, MoreNeuLayers);
			}
		}*/

		//if (network->getConfigVersion() < 21){
		//	for ( int n = 0; n < numCons; n++ ) {
		//		con = ((MDIChildren*) base)->GetCon(n);
				//((MDIChildren*) base)->ChartSetLoadConnection(con);
		//	}
		//}
		//((MDIChildren*) base)->ChartUpdateStatusDsp();
		//((MDIChildren*) base)->SetChartStatus();

	}//End of check that connections are present - if(va)

	//InvalidateFrames(true);


}


/*! Loads up the connection parameters from the specified file */
void NRMConfigLoader::loadConnectionParameters(conType& conParams, FILE* file){
	fReadFile(&conParams.randomCon, 1, 1, file);
	fReadFile(&conParams.fullCon, 1, 1, file);
	fReadFile(&conParams.spatialAlignment, 1, 1, file);
	fReadFile(&conParams.HFlipped, 1, 1, file);
	fReadFile(&conParams.VFlipped, 1, 1, file);
	fReadFile(&conParams.globalMap, 1, 1, file);
	fReadFile(&conParams.iconicMap, 1, 1, file);
	fReadFile(&conParams.segmentedMap, 1, 1, file);
	fReadFile(&conParams.gazeMap, 1, 1, file);
	fReadFile(&conParams.tileMap, 1, 1, file);
	fReadFile(&conParams.colScheme, 4, 1, file);
	fReadFile(&conParams.numCons, 4, 1, file);
	fReadFile(&conParams.sWidth, 4, 1, file);
	fReadFile(&conParams.sHeight, 4, 1, file);
	fReadFile(&conParams.sLeft, 4, 1, file);
	fReadFile(&conParams.sTop, 4, 1, file);
	fReadFile(&conParams.rWidth, 4, 1, file);
	fReadFile(&conParams.rHeight, 4, 1, file);
	fReadFile(&conParams.segRev, 1, 1, file);
}


