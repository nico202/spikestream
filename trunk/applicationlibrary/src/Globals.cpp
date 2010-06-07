//SpikeStream includes
#include "Globals.h"
#include "SpikeStreamException.h"
#include "SpikeStreamAnalysisException.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;


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
bool Globals::drawAxes = true;
float Globals::vertexSize = 7.5f;
bool Globals::simulationLoaded = false;
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
	if(archive != NULL){
		delete archive;
		archive = NULL;
	}
	if(network != NULL){
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


/*! Returns the root directory where SpikeStream is installed.*/
QString Globals::getSpikeStreamRoot(){
	return spikeStreamRoot;
}


/*! Returns the size of vertexes in the 3D Network Viewer.
	This is a configuration setting. */
float Globals::getVertexSize(){
	return vertexSize;
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


/*! Returns true if draw axes mode is set */
bool Globals::isDrawAxes(){
	return drawAxes;
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
	return Globals::simulationLoaded;
}


/*! Returns true if a simulation is running */
bool  Globals::isSimulationRunning(){
	return Globals::simulationRunning;
}


/*! Sets the analysis id. An id of 0 indicates that no analysis is loaded. */
void Globals::setAnalysisID(const QString& analysisName, unsigned int id){
	analysisMap[analysisName] = id;
}


/*! Records that a particular analysis is running */
void Globals::setAnalysisRunning(const QString &analysisName){
	analysisRunningMap[analysisName] = true;
}


/*! Sets parameter controlling whether axes are displayed in the 3D viewer */
void Globals::setDrawAxes(bool drawAxes){
	Globals::drawAxes = drawAxes;
}


/*! Sets the simulation loaded state */
void Globals::setSimulationLoaded(bool simulationLoaded){
	Globals::simulationLoaded = simulationLoaded;
}


/*! Sets the simulation running state */
void Globals::setSimulationRunning(bool simulationRunning){
	Globals::simulationRunning = simulationRunning;
}


/*! Sets the size of vertices in the 3D Network Viewer */
void Globals::setVertexSize(float vertexSize){
	Globals::vertexSize = vertexSize;
}



/*---------------------------------------------------------------------------------*/
/*----------                  PRIVATE METHODS                         -------------*/
/*---------------------------------------------------------------------------------*/

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


/*! Sets the root directory. */
void Globals::setSpikeStreamRoot(QString rootDir){
	spikeStreamRoot = rootDir;
}


/*! Sets the working directory */
void Globals::setWorkingDirectory(QString workDir){
	workingDirectory = workDir;
}



