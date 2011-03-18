//SpikeStream includes
#include "ISpikeManager.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <vector>
using namespace std;

/*! Constructor */
ISpikeManager::ISpikeManager() {

}


/*! Destructor */
ISpikeManager::~ISpikeManager(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Adds a channel, which will be a source of spikes to pass to the network
	or the destination of spikes that will be passed back to iSpike. */
void ISpikeManager::addChannel(InputChannel* inputChannel, NeuronGroup* neuronGroup){
	inputChannels.append(QPair<InputChannel*, NeuronGroup*>(inputChannel, neuronGroup));
}


/*! Adds an output channel, which will receive spikes from the network. */
void ISpikeManager::addChannel(OutputChannel* outputChannel, NeuronGroup* neuronGroup){
	outputChannels.append(QPair<OutputChannel*, NeuronGroup*>(outputChannel, neuronGroup));
}


/*! Deletes an input channel. */
void ISpikeManager::deleteInputChannel(int index){
	if(index < 0 || index >= inputChannels.size())
		throw SpikeStreamException("Failed to delete input channel: index out of range: " + QString::number(index));
	delete inputChannels.at(index).first;//FIXME, CHECK
	inputChannels.removeAt(index);
}


/*! Deletes an output channel. */
void ISpikeManager::deleteOutputChannel(int index){
	if(index < 0 || index >= outputChannels.size())
		throw SpikeStreamException("Failed to delete output channel: index out of range: " + QString::number(index));
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
	if(index < 0 || index >= inputChannels.size())
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
	//Array of vectors to hold the firing neuron ids for each channel
	vector<int> vectorArray[outputChannels.size()];

	for(QList<neurid_t>::iterator fireNeurIter = firingNeuronIDs.begin(); fireNeurIter != firingNeuronIDs.end(); ++fireNeurIter){
		//Find the neuron group
		for(int chanCtr = 0; chanCtr < outputChannels.size(); ++chanCtr){
			NeuronGroup* tmpNeurGrp = outputChannels.at(chanCtr).second;

			//Is this firing neuron id in this channel?
			if(*fireNeurIter >= tmpNeurGrp->getStartNeuronID() && *fireNeurIter <  tmpNeurGrp->getStartNeuronID() + tmpNeurGrp->size()){
				//Storing the firing id for that particular channel
				vectorArray[chanCtr].push_back(*fireNeurIter -  tmpNeurGrp->getStartNeuronID());
			}
		}
	}

	//Pass firing neuron ids to appropriate channels
	for(int chanCtr = 0; chanCtr < outputChannels.size(); ++chanCtr){
		if(!outputChannels[chanCtr].first->isInitialised())
			outputChannels[chanCtr].first->start();
		outputChannels[chanCtr].first->setFiring(&vectorArray[chanCtr]);
	}
}


//Inherited from AbstractDeviceManager
void ISpikeManager::step(){
	qDebug()<<"Stepping ISpikeManager";
	//Work through input channels
	outputNeuronIDs.clear();
	for(int chanCtr = 0; chanCtr < inputChannels.size(); ++chanCtr){
		if(!inputChannels[chanCtr].first->isInitialised())
			inputChannels[chanCtr].first->start();
		inputChannels[chanCtr].first->step();

		//Get firing from input channels
		vector< vector<int> > tmpFiringVect = inputChannels[chanCtr].first->getFiring();
		qDebug()<<"Number of stored time steps: "<<tmpFiringVect.size();
		if(tmpFiringVect.size() > 0)
			qDebug()<<"Number of firing neurons at time step 0 "<<tmpFiringVect[0].size();

		//Work through the firing neurons
		for(vector< vector<int> >::iterator outerIter = tmpFiringVect.begin(); outerIter != tmpFiringVect.end(); ++outerIter){
			for(vector<int>::iterator innerIter = outerIter->begin(); innerIter != outerIter->end(); ++innerIter){
				neurid_t tmpNeurID = *innerIter + inputChannels[chanCtr].second->getStartNeuronID();
				qDebug()<<"FIRING NEURON ID: "<<tmpNeurID;
				outputNeuronIDs.append(tmpNeurID);
			}
		}
	}
	//Work through output channels
	for(int chanCtr = 0; chanCtr < outputChannels.size(); ++chanCtr){
		if(!outputChannels[chanCtr].first->isInitialised())
			outputChannels[chanCtr].first->start();
		outputChannels[chanCtr].first->step();
	}
}

