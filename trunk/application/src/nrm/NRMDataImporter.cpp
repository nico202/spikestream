#include "NRMDataImporter.h"
#include "SpikeStreamException.h"
using namespace spikestream;

#include <iostream>
using namespace std;

/*! Constructor */
NRMDataImporter::NRMDataImporter(const DBInfo& dbInfo){
    currentTask = -1;
    networkDaoThread = new NetworkDaoThread(dbInfo);
}


/*! Destructor */
NRMDataImporter::~NRMDataImporter(){
}


/*! Sets class up to add connections to the database */
void NRMDataImporter::prepareAddConnections(NRMNetwork* nrmNetwork, Network* network){
    this->nrmNetwork = nrmNetwork;
    this->network = network;
    currentTask = ADD_CONNECTIONS_TASK;
}


/*! Inherited from QThread.  */
void NRMDataImporter::run(){
    stopThread = false;
    clearError();

    try{
	switch(currentTask){
	    case ADD_CONNECTIONS_TASK:
		addConnections();
	    break;
	    default:
		setError("Current task not recognized: " + QString::number(currentTask));
	}
    }
    catch(SpikeStreamException& ex){
	setError(ex.getMessage());
    }
    catch(...){
	setError("Unknown Exception thrown by DataImporter.");
    }

    currentTask = -1;
    stopThread = true;
}


/*! Stops thread and any other threads running within this thread. */
void NRMDataImporter::stop(){
    networkDaoThread->stop();
    stopThread = true;
}


/*! Adds connections to the SpikeStream network and then adds these connections to the database.
    Connections are directly added to the database and not added to the network, which is deleted anyway. */
void NRMDataImporter::addConnections(){
    //List of connection groups to be added to the database
    QList<ConnectionGroup*> newConnectionGrpsList;

    /* Work through the list of neural layers in the NRM network.
       Only the neural layers contain connections, but they can connect to input layers as well. */
    QList<NRMNeuralLayer*> neuralList = nrmNetwork->getAllNeuralLayers();
    for(int i=0; i<neuralList.size(); ++i){

	//Get pointer to the neural layer whose connections are being loaded
	NRMNeuralLayer* nrmToLayer = neuralList[i];

	//Get the list of connection groups that connect to this neural layer
	QList<NRMConnection*> nrmConnGrpList = nrmToLayer->getConnections();

	//Work through the connection groups to this neural layer
	for(int j=0; j<nrmConnGrpList.size(); ++j){

	    //Get a pointer to the connection group
	    NRMConnection* nrmConGrp = nrmConnGrpList[j];

	    /* Double check that the to neuron group is the to neuron group in the connection.
	       In NRM the src layer is the current layer. */
	    NRMLayer* tmpLayer = nrmNetwork->getLayerById(nrmConGrp->srcLayerId, nrmConGrp->srcObjectType);
	    if(tmpLayer != nrmToLayer)
		throw SpikeStreamException("To layer in connection does not match to neuron group.");

	    /* Get the layer that this connection is from .
		In NRM the src layer is the current layer and the dest layer is the layer
		connecting to the src current layer. */
	    NRMLayer* nrmFromLayer= nrmNetwork->getLayerById(nrmConGrp->destLayerId, nrmConGrp->destObjectType);

	    //Check that SpikeStream IDs have been set
	    if(nrmFromLayer->spikeStreamID == 0 || nrmToLayer->spikeStreamID == 0)
		throw SpikeStreamException("SpikeStream IDs have not been set in from and/or to NRM layers.");

	    //Get the from and to SpikeStream neuron groups
	    NeuronGroup* fromNeuronGroup = network->getNeuronGroup(nrmFromLayer->spikeStreamID);
	    NeuronGroup* toNeuronGroup = network->getNeuronGroup(nrmToLayer->spikeStreamID);

	    //Check that the sizes of the layers match
	    if(fromNeuronGroup->size() != nrmFromLayer->getSize() || toNeuronGroup->size() != nrmToLayer->getSize())
		throw SpikeStreamException("From and/or To neuron group sizes do not match.");

	    //Create a SpikeStream connection group
	    ConnectionGroup* newConGrp = new ConnectionGroup( ConnectionGroupInfo(0, "Untitled", nrmFromLayer->spikeStreamID, nrmToLayer->spikeStreamID, QHash<QString, double>(),2) );

	    //Work through the neurons in the to layer and add their connections to the new SpikeStream group
	    unsigned int fromNeuronID = 0, toNeuronID = 0;
	    int toNeurGrpSize = nrmToLayer->getSize();//Saves accessing it every time in the loop
	    for(int neurNum = 0; neurNum < toNeurGrpSize; ++neurNum){
		//Get the connections of this neuron
		QList<unsigned int> neuronConns = nrmConGrp->getNeuronConnections(neurNum);

		//Work through the connections of the neuron
		QList<unsigned int>::iterator endNeuronConns = neuronConns.end();
		for(QList<unsigned int>::iterator iter = neuronConns.begin(); iter != endNeuronConns; ++iter){

		    /* The neuron ids are offsets from the first neuron id in the from group.*/
		    fromNeuronID = *iter + fromNeuronGroup->getStartNeuronID();
		    toNeuronID = neurNum + toNeuronGroup->getStartNeuronID();

		    //Double check that neuron ids are valid
		    if(fromNeuronID == 0 || toNeuronID == 0)
			throw SpikeStreamException("Invalid from and/or to neuron IDs.");

		    //Add the connection to the connection group
		    Connection* newConn = new Connection(fromNeuronID, toNeuronID, 0, 0, 0);
		    newConGrp->addConnection(newConn);
		}
	    }

	    //Add the connection group to the list
	    newConnectionGrpsList.append(newConGrp);

	}//Finished working through the connections associated with one NRM neural layer

    }//Finished working through all of the NRM neural layers

    //Add connections to the database
    networkDaoThread->prepareAddConnectionGroups(network->getID(), newConnectionGrpsList);
    networkDaoThread->start();

    /* Wait for thread to finish - this task is already running as a separate thread
	and has nothing else to do, so waiting will not affect the main application.*/
    networkDaoThread->wait();

    //Check for errors
    if(networkDaoThread->isError())
	setError(networkDaoThread->getErrorMessage());
}


/*! Clears the error state of the thread. */
void NRMDataImporter::clearError(){
    errorMessage = "";
    error = false;
}


/*! Exceptions do not work across threads, so errors are flagged by calling this method.
    The invoking method is responsible for checking whether an error occurred and throwing
    an exeption if necessary.*/
void NRMDataImporter::setError(const QString& msg){
    errorMessage = msg;
    error = true;
    stopThread = true;
}
