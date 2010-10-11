//SpikeStream includes
#include "AbstractConnectionBuilder.h"
#include "Globals.h"
#include "Network.h"
#include "SpikeStreamException.h"
using namespace spikestream;

/*! Constructor */
AbstractConnectionBuilder::AbstractConnectionBuilder(){
}


/*! Destructor */
AbstractConnectionBuilder::~AbstractConnectionBuilder(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Thread run method */
void AbstractConnectionBuilder::run(){
	clearError();
	stopThread = false;
	newConnectionGroup = NULL;

	try{
		//Seed the random number generator
		srand(12345678);

		//Create network and archive dao for this thread
		Network* currentNetwork = Globals::getNetwork();
		threadNetworkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());

		//Build connection group and add it to network
		buildConnectionGroup();
		QList<ConnectionGroup*> conGrpList;
		conGrpList.append(newConnectionGroup);
		Globals::getNetwork()->addConnectionGroups(conGrpList);

		//Wait for network to finish adding connection groups
		while(currentNetwork->isBusy()){
			emit progress(threadNetworkDao->getConnectionCount(newConnectionGroup), newConnectionGroup->size(), "Adding connections to database...");
			msleep(250);
		}

		//Check for errors
		if(currentNetwork->isError())
			setError(currentNetwork->getErrorMessage());

		//Clean up network dao
		delete threadNetworkDao;
	}
	catch (SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("An unknown error occurred.");
	}
}


/*! Prepares class before it runs as a separate thread to add a neuron group */
void AbstractConnectionBuilder::startBuildConnectionGroup(const ConnectionGroupInfo& conGrpInfo){
	//Store information about the neuron group to be added
	this->connectionGroupInfo = conGrpInfo;

	//Check that the required parameters exist in the connection group and are in the appropriate range
	checkParameters();

	if(!Globals::networkLoaded())
		throw SpikeStreamException("Cannot add connection group - no network loaded.");

	this->start();
}


/*----------------------------------------------------------*/
/*-----                PROTECTED METHODS               -----*/
/*----------------------------------------------------------*/

/*! Returns a parameter from the connection group info parameter map checking that it actually exists */
double AbstractConnectionBuilder::getParameter(const QString& paramName){
	if(!connectionGroupInfo.hasParameter(paramName))
		throw SpikeStreamException("Parameter with " + paramName + " does not exist in connection group's parameter map.");
	return connectionGroupInfo.getParameter(paramName);
}

