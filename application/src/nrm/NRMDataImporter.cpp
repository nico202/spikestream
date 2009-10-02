#include "ArchiveInfo.h"
#include "ArchiveDao.h"
#include "NRMDataImporter.h"
#include "NRMException.h"
#include "SpikeStreamException.h"
using namespace spikestream;

#include <iostream>
using namespace std;

/*! Constructor */
NRMDataImporter::NRMDataImporter(const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo){
    currentTask = -1;
    networkDaoThread = new NetworkDaoThread(networkDBInfo);
    this->archiveDBInfo = archiveDBInfo;
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


/*! Sets class up to add archives to the database */
void NRMDataImporter::prepareAddArchives(NRMNetwork* nrmNetwork, Network* network, NRMDataSet* nrmDataset){
    this->nrmNetwork = nrmNetwork;
    this->network = network;
    this->nrmDataset = nrmDataset;

    //Run some basic checks
    if(nrmDataset->size() % nrmNetwork->getNumberOfLayers() != 0)
	throw NRMException("Number of entries in dataset does not divide exactly into the number of layers in the NRM network. Too few or too many data set entries.");
    currentTask = ADD_ARCHIVES_TASK;
}


/*! Inherited from QThread.  */
void NRMDataImporter::run(){
    stopThread = false;
    clearError();

    try{
	switch(currentTask){
	    case ADD_ARCHIVES_TASK:
		addArchives();
	    break;
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


/*! Adds archives to the archive database.
    Each dataset is assigned to the input layers in ascending order, then the neural layers,
    then the cycle is repeated again. Mismatch in the numbers should be detected when this
    task is prepared */
void NRMDataImporter::addArchives(){
    //Add a new archive to the database
    ArchiveInfo archiveInfo(0, network->getID(), QDateTime::currentDateTime().toTime_t(), "NRM imported dataset");
    ArchiveDao archiveDao(archiveDBInfo);
    archiveDao.addArchive(archiveInfo);

    //Get the list of data arrays from the data set
    QList<unsigned char*> dataList = nrmDataset->getDataList();

    //Keeps track of where we are in the data list
    unsigned int dataListCtr = 0;

    //Check that there is an exact division between the number of layers and the size of the dataset
    if(dataList.size() % nrmNetwork->getNumberOfLayers() != 0)
	throw NRMException("Data sets do not match layers in NRM network");

    //Work through the dataset one time step at a time. Each time step is added as an entry to the archive data
    for(int timeStepCtr=1; timeStepCtr <= ( dataList.size() / nrmNetwork->getNumberOfLayers() ); ++timeStepCtr){

	//String containing the firing neurons at this time step
	QString firingNeuronString = "";

	//Work through the input layers
	QList<NRMInputLayer*> inputList = nrmNetwork->getAllInputs();
	for(int layerCtr=0; layerCtr<inputList.size(); ++layerCtr){
	    NRMLayer* nrmLayer = inputList[layerCtr];

	    //Get the spikestream neuron group corresponding to this NRM layer
	    NeuronGroup* neurGrp = network->getNeuronGroup(nrmLayer->spikeStreamID);

	    //Check that the size matches the dataset
	    if(neurGrp->size() != (nrmDataset->width * nrmDataset->height))
		throw NRMException("Dataset does not match neuron group size.");

	    //Add firing neurons to the string
	    for(int neurCtr = 0; neurCtr < neurGrp->size(); ++neurCtr){
		if(dataList[dataListCtr][neurCtr] != 0){//Typically a firing neuron has value 7
		    firingNeuronString += QString::number(neurGrp->getStartNeuronID() + neurCtr) + ",";
		}
	    }

	    //Increase counter keeping track of where we are in the datasets
	    ++dataListCtr;
	}

	//Work through the neural layers
	QList<NRMNeuralLayer*> neuralList = nrmNetwork->getAllNeuralLayers();
	for(int layerCtr=0; layerCtr<neuralList.size(); ++layerCtr){
	    NRMLayer* nrmLayer = neuralList[layerCtr];

	    //Get the spikestream neuron group corresponding to this NRM layer
	    NeuronGroup* neurGrp = network->getNeuronGroup(nrmLayer->spikeStreamID);

	    //Check that the size matches the dataset
	    if(neurGrp->size() != (nrmDataset->width * nrmDataset->height))
		throw SpikeStreamException("Dataset does not match neuron group size.");

	    //Add firing neurons to the string
	    for(int neurCtr=0; neurCtr < neurGrp->size(); ++neurCtr){
		if(dataList[dataListCtr][neurCtr] != 0){
		    firingNeuronString += QString::number(neurGrp->getStartNeuronID() + neurCtr) + ",";
		}
	    }

	    //Increase counter keeping track of where we are in the datasets
	    ++dataListCtr;
	}

	//trim trailing comma off firing string
	if(firingNeuronString.length() > 0)
	    firingNeuronString.truncate(firingNeuronString.length() - 1);

	//Add the archive data to the database
	archiveDao.addArchiveData(archiveInfo.getID(), timeStepCtr, firingNeuronString);
    }
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
