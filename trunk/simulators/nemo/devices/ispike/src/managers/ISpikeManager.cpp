//SpikeStream includes
#include "ISpikeManager.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <vector>
using namespace std;

//#define DEBUG

/*! Constructor */
ISpikeManager::ISpikeManager() {
}


/*! Destructor */
ISpikeManager::~ISpikeManager(){
	deleteAllChannels();
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Adds an input channel, which will be a source of spikes to pass to the network. */
void ISpikeManager::addChannel(InputChannel* inputChannel, NeuronGroup* neuronGroup){
	inputChannels.append(QPair<InputChannel*, NeuronGroup*>(inputChannel, neuronGroup));

	//Create an array whose index is the iSpike neuron id and whose value is the SpikeStream neuron ID
	neurid_t* tmpNeurIDArray = new neurid_t[neuronGroup->size()];
	int iSpikeIDCtr = 0;
	NeuronPositionIterator neurGrpEnd = neuronGroup->positionEnd();
	for(NeuronPositionIterator posIter = neuronGroup->positionBegin(); posIter != neurGrpEnd; ++posIter){
		tmpNeurIDArray[iSpikeIDCtr] = posIter.value()->getID();
		++iSpikeIDCtr;
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
}


/*! Deletes an input channel. */
void ISpikeManager::deleteInputChannel(int index){
	if(index < 0 || index >= inputChannels.size())
		throw SpikeStreamException("Failed to delete input channel: index out of range: " + QString::number(index));
	delete inputChannels.at(index).first;//Delete iSpike input channel
	delete inputArrayList.at(index);//Delete array linking iSpike IDs to SpikeStream IDs
	inputArrayList.removeAt(index);
	inputChannels.removeAt(index);
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
map<string, Property*> ISpikeManager::getInputParameters(int index){
	if(index < 0 || index >= inputChannels.size())
		throw SpikeStreamException("Failed to get input parameters: index out of range: " + QString::number(index));
	return inputChannels[index].first->getChannelDescription().getChannelProperties();
}


/*! Returns the parameters for an output channel. The index should be a valid
 point in the output channels list. */
map<string, Property*> ISpikeManager::getOutputParameters(int index){
	if(index < 0 || index >= outputChannels.size())
		throw SpikeStreamException("Failed to get input parameters: index out of range: " + QString::number(index));
	return outputChannels[index].first->getChannelDescription().getChannelProperties();
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
			if(!outputChannels[chanCtr].first->isInitialised())
				outputChannels[chanCtr].first->start();
			outputChannels[chanCtr].first->setFiring(&firingNeurIDVectors[chanCtr]);
		}
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

	//Work through input channel
	try{
		for(int chanCtr = 0; chanCtr < inputChannels.size(); ++chanCtr){
			if(!inputChannels[chanCtr].first->isInitialised())
				inputChannels[chanCtr].first->start();
			inputChannels[chanCtr].first->step();

			//Get firing from input channels. CAN THIS BE A REFERENCE?
			vector< vector<int> > tmpFiringVect = inputChannels[chanCtr].first->getFiring();

			//Work through the firing neurons
			for(vector< vector<int> >::iterator outerIter = tmpFiringVect.begin(); outerIter != tmpFiringVect.end(); ++outerIter){
				for(vector<int>::iterator innerIter = outerIter->begin(); innerIter != outerIter->end(); ++innerIter){
					#ifdef DEBUG
						qDebug()<<"FIRING NEURON: iSpikeID: "<<*innerIter<<"; SpikeStream ID: "<<inputArrayList[chanCtr][*innerIter];
					#endif//DEBUG
					outputNeuronIDs.append(inputArrayList[chanCtr][*innerIter]);
				}
			}
		}
		//Step output channelss
		for(int chanCtr = 0; chanCtr < outputChannels.size(); ++chanCtr){
			if(!outputChannels[chanCtr].first->isInitialised())
				outputChannels[chanCtr].first->start();
			outputChannels[chanCtr].first->step();
		}
	}
	catch(...){
		throw SpikeStreamException("An unknown exception occurred stepping iSpike channels.");
	}
}

