//SpikeStream includes
#include "Globals.h"
#include "ISpikeManager.h"
#include "SpikeStreamException.h"
#include "SpikeStreamIOException.h"
using namespace spikestream;

//iSpike includes
#include "iSpike/ISpikeException.hpp"

//Qt includes
#include <QDebug>

//Other includes
#include <vector>
using namespace std;

//Debug output
//#define DEBUG
//#define DEBUG_NEURON_IDS


/*! Constructor */
ISpikeManager::ISpikeManager() : AbstractDeviceManager() {
	#ifdef DEBUG_NEURON_IDS
		logFile = new QFile(Globals::getSpikeStreamRoot() + "/log/ISpikeManager.log");
		if(logFile->open(QFile::WriteOnly | QFile::Truncate))
			logTextStream = new QTextStream(logFile);
		else{
			throw SpikeStreamIOException("Cannot open log file for ISpikeManager.");
		}
	#endif//DEBUG_NEURON_IDS
}


/*! Destructor */
ISpikeManager::~ISpikeManager(){
	deleteAllChannels();
	//Clean up log file if logging is enabled
	#ifdef DEBUG_NEURON_IDS
		logFile->close();
		delete logFile;
		delete logTextStream;
	#endif//DEBUG_NEURON_IDS
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Adds an input channel, which will be a source of spikes to pass to the network. */
void ISpikeManager::addChannel(InputChannel* inputChannel, NeuronGroup* neuronGroup){
	//Check channel matches neuron group
	if(inputChannel->size() != neuronGroup->size())
		throw SpikeStreamException("Input channel size does not match size of neuron group.");

	//Store the channel
	inputChannels.append(QPair<InputChannel*, NeuronGroup*>(inputChannel, neuronGroup));
	int tmpWidth = inputChannel->getWidth();
	int tmpHeight = inputChannel->getHeight();
	int tmpX, tmpZ;

	//Create an array whose index is the iSpike neuron id and whose value is the SpikeStream neuron ID
	neurid_t* tmpNeurIDArray = new neurid_t[neuronGroup->size()];
	int ctr = 0;
	NeuronPositionIterator neurGrpEnd = neuronGroup->positionEnd();
	for(NeuronPositionIterator posIter = neuronGroup->positionBegin(); posIter != neurGrpEnd; ++posIter){
		//Get X and Z position (assume layer is mounted vertically)
		tmpX = ctr/tmpHeight;
		tmpZ = ctr%tmpHeight;

		//Convert X and Z positions to ensure topographic mapping with image
		tmpNeurIDArray[tmpZ * tmpWidth + tmpX] = posIter.value()->getID();

		//Output debugging information if required
		#ifdef DEBUG_NEURON_IDS
			(*logTextStream)<<"iSpike ID: "<<(tmpZ * tmpWidth + tmpX)<<"; SpikeStream ID: "<<posIter.value()->getID()<<"; SpikeStream Position: ("<<posIter.value()->getXPos()<<", "<<posIter.value()->getYPos()<<", "<<posIter.value()->getZPos()<<")"<<endl;
		#endif//DEBUG_NEURON_IDS
		++ctr;
	}
	inputArrayList.append(tmpNeurIDArray);
}


/*! Adds an output channel, which will receive spikes from the network. */
void ISpikeManager::addChannel(OutputChannel* outputChannel, NeuronGroup* neuronGroup){
	//Check that neuron group is not already in use - currently only support one neuron group per channel
	if(neurIDMap.contains(neuronGroup->getStartNeuronID()))
		throw SpikeStreamException("Neuron groups cannot be connected to multiple output channels.");

	//Store output channel
	outputChannels.append(QPair<OutputChannel*, NeuronGroup*>(outputChannel, neuronGroup));

	//Create vector to hold neuron IDs passed to this output channel
	firingNeurIDVectors.push_back(vector<int>());

	/* Add entries to neuron ID map linking SpikeStream ID with start neuron ID and input vector.
		This has to be done topographically because neuron ids do not increase smoothly with distance. */
	int startMapSize = neurIDMap.size();
	int iSpikeIDCtr = 0;
	NeuronPositionIterator neurGrpEnd = neuronGroup->positionEnd();
	for(NeuronPositionIterator posIter = neuronGroup->positionBegin(); posIter != neurGrpEnd; ++posIter){
		QPair<neurid_t, vector<int>*>* channelDetails = new QPair<neurid_t, vector<int>*>(iSpikeIDCtr, &firingNeurIDVectors.back());
		neurIDMap[posIter.value()->getID()] = channelDetails;
		++iSpikeIDCtr;
	}

	//Check that IDs are in a continuous range.
	if(startMapSize + neuronGroup->size() != neurIDMap.size())
		throw SpikeStreamException("Neuron ID error in neuron group " + QString::number(neuronGroup->getID()));
}


/*! Deletes all input and output channels */
void ISpikeManager::deleteAllChannels(){
	for(int i=0; i<inputChannels.size(); ++i){
		deleteInputChannel(i);
	}
	for(int i=0; i<outputChannels.size(); ++i){
		deleteOutputChannel(i);
	}
	inputChannels.clear();
	outputChannels.clear();
}


/*! Deletes an input channel. */
void ISpikeManager::deleteInputChannel(int index){
	if(index < 0 || index >= inputChannels.size())
		throw SpikeStreamException("Failed to delete input channel: index out of range: " + QString::number(index));
	delete inputChannels.at(index).first;//Delete iSpike input channel
	delete [] inputArrayList.at(index);//Delete array linking iSpike IDs to SpikeStream IDs
	inputArrayList.removeAt(index);
	inputChannels.removeAt(index);
	outputNeuronIDs.clear();//Make sure that there are not any old neuron IDs around
}


/*! Deletes an output channel. */
void ISpikeManager::deleteOutputChannel(int index){
	//Check index
	if(index < 0 || index >= outputChannels.size())
		throw SpikeStreamException("Failed to delete output channel: index out of range: " + QString::number(index));

	//Remove details about neuron group associated with output channel
	NeuronGroup* tmpNeurGrp = outputChannels.at(index).second;
	for(NeuronIterator iter = tmpNeurGrp->begin(); iter != tmpNeurGrp->end(); ++iter){
		delete neurIDMap[iter.key()];//Remove dynamically allocated QPair
		neurIDMap.remove(iter.key());
	}

	//Remove input vector associated with channel
	firingNeurIDVectors.erase(firingNeurIDVectors.begin() + index);

	//Delete iSpike channel from memory and remove output channel from list
	delete outputChannels.at(index).first;//FIXME, CHECK
	outputChannels.removeAt(index);
}


/*! Returns the parameters for an input channel. The index should be a valid
 point in the input channels list. */
map<string, Property> ISpikeManager::getInputProperties(int index){
	if(index < 0 || index >= inputChannels.size())
		throw SpikeStreamException("Failed to get input parameters: index out of range: " + QString::number(index));
	return inputChannels[index].first->getProperties();
}


/*! Returns the parameters for an output channel. The index should be a valid
 point in the output channels list. */
map<string, Property> ISpikeManager::getOutputProperties(int index){
	if(index < 0 || index >= outputChannels.size())
		throw SpikeStreamException("Failed to get input parameters: index out of range: " + QString::number(index));
	return outputChannels[index].first->getProperties();
}


/*! Sets the properties of a particular input channel */
void ISpikeManager::setInputProperties(int index, map<string, Property> &propertyMap){
	if(index < 0 || index >= inputChannels.size())
		throw SpikeStreamException("Failed to set input properties index out of range: " + QString::number(index));
	inputChannels[index].first->setProperties(propertyMap);
}


/*! Sets the properties of a particular input channel */
void ISpikeManager::setOutputProperties(int index, map<string, Property> &propertyMap){
	if(index < 0 || index >= outputChannels.size())
		throw SpikeStreamException("Failed to set output properties index out of range: " + QString::number(index));
	outputChannels[index].first->setProperties(propertyMap);
}


//Inherited from AbstractDeviceManager
QList<neurid_t>::iterator ISpikeManager::outputNeuronsBegin(){
	return outputNeuronIDs.begin();
}


//Inherited from AbstractDeviceManager
QList<neurid_t>::iterator ISpikeManager::outputNeuronsEnd(){
	return outputNeuronIDs.end();
}


//Inherited from AbstractDeviceManager
void ISpikeManager::setInputNeurons(timestep_t timeStep, QList<neurid_t>& firingNeuronIDs){
	#ifdef DEBUG
		qDebug()<<"Setting firing neurons from SpikeStream. num firing neurons: "<<firingNeuronIDs.size()<<"; num output channels: "<<outputChannels.size()<<"; num firing neuron ID vectors: "<<firingNeurIDVectors.size();
	#endif//DEBUG

	//Clear vectors holding firing neuron ids for each channel
	for(size_t i=0; i< firingNeurIDVectors.size(); ++i)
		firingNeurIDVectors[i].clear();

	//Work through firing neurons and add them to the appropriate input vector for the channel without the start neuron ID.
	QList<neurid_t>::iterator firingNeuronsEnd = firingNeuronIDs.end();
	for(QList<neurid_t>::iterator fireNeurIter = firingNeuronIDs.begin(); fireNeurIter != firingNeuronsEnd; ++fireNeurIter){
		if(neurIDMap.contains(*fireNeurIter))//Some neuron groups may not be connected to channels
			neurIDMap[*fireNeurIter]->second->push_back(neurIDMap[*fireNeurIter]->first);
	}

	try{
		//Pass firing neuron ids to appropriate channels
		for(int chanCtr = 0; chanCtr < outputChannels.size(); ++chanCtr){
			outputChannels[chanCtr].first->setFiring(firingNeurIDVectors[chanCtr]);
		}
	}
	catch(ISpikeException& ex){
		throw SpikeStreamException("ISpikeManager: Error occurred setting input neurons: " + QString(ex.what()));
	}
	catch(...){
		throw SpikeStreamException("ISpikeManager: An unknown exception occurred setting input neurons.");
	}
}


//Inherited from AbstractDeviceManager
void ISpikeManager::step(){
	#ifdef DEBUG
		qDebug()<<"Stepping ISpikeManager. Num output channels: "<<outputChannels.size()<<"; num input channels: "<<inputChannels.size();
	#endif//DEBUG
	outputNeuronIDs.clear();

	try{
		//Work through input channels - these pass spikes TO the SpikeStream network
		for(int chanCtr = 0; chanCtr < inputChannels.size(); ++chanCtr){
			inputChannels[chanCtr].first->step();
			vector<int>& tmpFiringVect = inputChannels[chanCtr].first->getFiring();

			//Work through the firing neurons
			for(vector<int>::iterator iter = tmpFiringVect.begin(); iter != tmpFiringVect.end(); ++iter){
				#ifdef DEBUG
					qDebug()<<"FIRING NEURON: iSpikeID: "<<*innerIter<<"; SpikeStream ID: "<<inputArrayList[chanCtr][*iter];
				#endif//DEBUG

				//Convert iSpike ID to SpikeStream ID
				outputNeuronIDs.append(inputArrayList[chanCtr][*iter]);
			}
		}

		//Step output channels - these receive spikes FROM the SpikeStream network
		for(int chanCtr = 0; chanCtr < outputChannels.size(); ++chanCtr){
			outputChannels[chanCtr].first->step();
		}
	}
	catch(ISpikeException& ex){
		throw SpikeStreamException("ISpikeManager: Error occurred stepping channels: " + QString(ex.what()));
	}
	catch(...){
		throw SpikeStreamException("An unknown exception occurred stepping iSpike channels.");
	}
}

