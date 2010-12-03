//SpikeStream includes
#include "Globals.h"
#include "SpikeStreamException.h"
#include "SpikeStreamAnalysisException.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

//Define controlling output of memory information
#define MEMORY_DEBUG

//Declare static variables
QHash<QString, unsigned int> Globals::analysisMap;
QHash<QString, bool> Globals::analysisRunningMap;
AnalysisDao* Globals::analysisDao = NULL;
Archive* Globals::archive = NULL;
ArchiveDao* Globals::archiveDao = NULL;
bool Globals::archivePlaying = false;
EventRouter* Globals::eventRouter = new EventRouter();
Network* Globals::network = NULL;
NetworkDao* Globals::networkDao = NULL;
NetworkDisplay* Globals::networkDisplay = new NetworkDisplay();
bool Globals::rendering = false;
QString Globals::spikeStreamRoot = "";
QString Globals::workingDirectory = "";
AbstractSimulation* Globals::simulation = NULL;
bool Globals::simulationRunning = false;


/*---------------------------------------------------------------------------------*/
/*----------                  PUBLIC METHODS                          -------------*/
/*---------------------------------------------------------------------------------*/

/*! Clears the running state for a particular analysis */
void Globals::clearAnalysisRunning(const QString &analysisName){
	analysisRunningMap.remove(analysisName);
}


/*! Returns true if an analysis is loaded */
bool Globals::isAnalysisLoaded(const QString& analysisName){
	//Map has an entry for this analysis - return false if ID is zero
	if(analysisMap.contains(analysisName)){
		if(analysisMap[analysisName] == 0)
			return false;
		return true;
	}

	//No entry in map so no analysis loaded
	return false;
}


/*! Returns true if any analysis is running */
bool Globals::isAnalysisRunning(){
	//Analysis running map is empty, so no analyses are running
	if(analysisRunningMap.isEmpty())
		return false;
	return true;
}


/*! Returns the analysis id if it has been set, otherwise return zero */
unsigned int Globals::getAnalysisID(const QString& analysisName){
	if(analysisMap.contains(analysisName))
		return analysisMap[analysisName];

	//No analysis ID has been set for this name, so return zero
	return 0;
}


/*! Returns true if an archive is loaded.
	Should always check with this method before requesting the archive because it
	will be null if it is not loaded. */
bool Globals::archiveLoaded(){
	if(archive == NULL)
		return false;
	return true;
}


/*! Cleans up all classes stored in Globals.
	Everything stored in Globals is deleted by Globals. */
void Globals::cleanUp(){
	#ifdef MEMORY_DEBUG
		cout<<"Cleaning up Globals"<<endl;
	#endif//MEMORY_DEBUG

	if(archive != NULL){
		delete archive;
		archive = NULL;
	}
	if(network != NULL){
		//Delete network if it has been marked as temporary and has not been saved.
		if(network->isTransient() && !network->isSaved()){
			NetworkDaoThread networkDaoThread(networkDao->getDBInfo());
			networkDaoThread.startDeleteNetwork(network->getID());
			networkDaoThread.wait();
		}
		delete network;
		network = NULL;
	}
	if(networkDao != NULL){
		delete networkDao;
		networkDao = NULL;
	}
	if(eventRouter != NULL){
		delete eventRouter;
		eventRouter = NULL;
	}
}


/*! Returns the analysis Dao that wraps the SpikeStreamAnalysis database. */
AnalysisDao* Globals::getAnalysisDao(){
	return analysisDao;
}


/*! Returns the current archive */
Archive* Globals::getArchive(){
	if(archive == NULL)
		throw SpikeStreamException("No archive loaded. You should check that archive is loaded using archiveLoaded() before calling this method.");
	return archive;
}


/*! Returns the archive dao, which provides a layer of abstraction on top of the
	SpikeStreamArchive database */
ArchiveDao* Globals::getArchiveDao(){
	return archiveDao;
}


/*! Returns the current network */
Network* Globals::getNetwork(){
	if(network == NULL)
		throw SpikeStreamException("No network loaded. You should check that network is loaded using networkLoaded() before calling this method.");
	return network;
}


/*! Returns the network dao, which provides a layer of abstraction on top of the
	SpikeStreamNetwork database */
NetworkDao* Globals::getNetworkDao(){
	return networkDao;
}


/*! Returns the network display. */
NetworkDisplay* Globals::getNetworkDisplay(){
	return networkDisplay;
}


/*! Returns the simulation */
AbstractSimulation* Globals::getSimulation(){
	return Globals::simulation;
}


/*! Returns the root directory where SpikeStream is installed.*/
QString Globals::getSpikeStreamRoot(){
	return spikeStreamRoot;
}


/*! Returns the working directory which is opened when loading projects, import files etc. */
QString Globals::getWorkingDirectory(){
	return workingDirectory;
}


/*! Returns true if a network is loaded, false if no network exists. */
bool Globals::networkLoaded(){
	if(network == NULL)
		return false;
	return true;
}


/*! Returns true if OpenGL rendering is in progress */
bool Globals::isRendering() {
	return rendering;
}


/*! Returns true if archive playback is in progress */
bool Globals::isArchivePlaying() {
	return archivePlaying;
}


/*! Returns true if a simulation is being loaded */
bool  Globals::isSimulationLoaded(){
	if(simulation == NULL)
		return false;
	return true;
}


/*! Returns true if a simulation is running */
bool  Globals::isSimulationRunning(){
	return Globals::simulationRunning;
}

/*! Runs checks when a network is loaded and the user attempts to load a different network or delete the current network.
	If a simulation or analysis is running, the user is requested to stop them and try again.
	If a simulation is loaded, confirmation is requested from the user.
	True is returned if everything is ok. */
bool Globals::networkChangeOk(){
	//If no network is loaded, then simulation or analysis will not be running
	if(!Globals::networkLoaded())
		return true;

	//Cannot change network when simulation is running
	if(Globals::isSimulationRunning()){
		qWarning()<<"Network cannot be changed or deleted whilst a simulation is running.\nStop the simulation and try again.";
		return false;
	}

	//Cannot change network when analysis is running
	if(Globals::isAnalysisRunning()){
		qWarning()<<"Network cannot be changed or deleted whilst an analysis is running.\nStop all analyses and try again.";
		return false;
	}

	//Cannot change network when archive is playing
	if(Globals::isArchivePlaying()){
		qWarning()<<"Network cannot be changed or deleted whilst an archive is playing.\nStop archive playback and try again.";
		return false;
	}

	//Check that user wants to unload simulation
	if(Globals::isSimulationLoaded()){
		qWarning()<<"A simulation is currently loaded.\nUnload the simulation before changing or deleting the current network.";
		return false;
	}

	//Network change is ok if we have reached this point
	return true;
}


/*! Sets the analysis id. An id of 0 indicates that no analysis is loaded. */
void Globals::setAnalysisID(const QString& analysisName, unsigned int id){
	analysisMap[analysisName] = id;
}


/*! Records that a particular analysis is running */
void Globals::setAnalysisRunning(const QString &analysisName){
	analysisRunningMap[analysisName] = true;
}


/*! Sets the simulation running state */
void Globals::setSimulationRunning(bool simulationRunning){
	Globals::simulationRunning = simulationRunning;
}


/*-------------------------------------------------------------*/
/*------                 PRIVATE METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Sets the analysis Dao */
void Globals::setAnalysisDao(AnalysisDao* newAnalysisDao){
	//Clean up the old analysis DAO if it exists
	if(Globals::analysisDao != NULL)
		delete analysisDao;

	Globals::analysisDao = newAnalysisDao;
}


/*! Sets the archive */
void Globals::setArchive(Archive* arch){
	//Clean up the old network if it exists
	if(Globals::archive != NULL)
		delete Globals::archive;

	//Store reference to the new network.
	Globals::archive = arch;
}


/*! Sets the archive dao. */
void Globals::setArchiveDao(ArchiveDao* archiveDao){
	//Clean up the old network DAO if it exists
	if(Globals::archiveDao != NULL)
		delete archiveDao;

	Globals::archiveDao = archiveDao;
}


/*! Sets whether archive playback is taking place */
void Globals::setArchivePlaying(bool archivePlaying){
	Globals::archivePlaying = archivePlaying;
}


/*! Sets the event router */
void Globals::setEventRouter(EventRouter* eventRouter){
	//Clean up the old network if it exists
	if(Globals::eventRouter != NULL)
		delete eventRouter;

	Globals::eventRouter = eventRouter;
}


/*! Sets the network */
void Globals::setNetwork(Network* net){
	//Clean up the old network if it exists
	if(Globals::network != NULL)
		delete Globals::network;

	//Store reference to the new network.
	Globals::network = net;
}


/*! Sets the class controlling the display of the network */
void Globals::setNetworkDisplay(NetworkDisplay* networkDisplay){
	//Clean up the old network display if it exists
	if(Globals::networkDisplay != NULL)
		delete networkDisplay;

	//Store reference to the new network display
	Globals::networkDisplay = networkDisplay;
}


/*! Sets the network dao. */
void Globals::setNetworkDao(NetworkDao* networkDao){
	//Clean up the old network DAO if it exists
	if(Globals::networkDao != NULL)
		delete networkDao;

	Globals::networkDao = networkDao;
}


/*! Sets whether the OpenGL rendering is taking place or is complete */
void Globals::setRendering(bool rendering){
	Globals::rendering = rendering;
}


/*! Sets the simulation.
	It is the caller's responsibilty to delete the simulation. */
void Globals::setSimulation(AbstractSimulation* simulation){
	Globals::simulation = simulation;
}


/*! Sets the root directory. */
void Globals::setSpikeStreamRoot(QString rootDir){
	spikeStreamRoot = rootDir;
}


/*! Sets the working directory */
void Globals::setWorkingDirectory(QString workDir){
	workingDirectory = workDir;
}



