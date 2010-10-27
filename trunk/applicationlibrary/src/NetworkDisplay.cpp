//SpikeStream includes
#include "Globals.h"
#include "GlobalVariables.h"
#include "NetworkDisplay.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;


/*! Constructor */
NetworkDisplay::NetworkDisplay(){
	//Reset this class when the network is changed
	connect(Globals::getEventRouter(), SIGNAL (networkChangedSignal()), this, SLOT(networkChanged()), Qt::QueuedConnection);

	//Inform other classes when the display has changed
	connect(this, SIGNAL(networkDisplayChanged()), Globals::getEventRouter(), SLOT(networkDisplayChangedSlot()), Qt:: QueuedConnection);
	connect(this, SIGNAL(neuronGroupDisplayChanged()), Globals::getEventRouter(), SLOT(neuronGroupDisplayChangedSlot()), Qt:: QueuedConnection);

	//Listen for changes to network viewer
	connect(Globals::getEventRouter(), SIGNAL(networkViewChangedSignal()), this, SLOT(clearZoom()));

	/* Set colours of neurons that are used multiple times and need to be stored in the default
	map to prevent deletion */
	archiveFiringNeuronColor.set(1.0f, 0.0f, 1.0f);
	defaultColorMap[&archiveFiringNeuronColor] = true;
	simulationFiringNeuronColor.set(0.0f, 1.0f, 0.0f);
	defaultColorMap[&simulationFiringNeuronColor] = true;
	highlightNeuronColor.set(0.0f, 1.0f, 1.0f);
	defaultColorMap[&highlightNeuronColor] = true;

	//Set neuron colours that are used directly by NetworkViewer and do not need to be stored in map
	defaultNeuronColor.set(0.0f, 0.0f, 0.0f);
	defaultNeuronColorFullRender.set(0.5f, 0.5f, 0.5f);
	singleNeuronColor.set(0.0f, 1.0f, 0.0f);
	toNeuronColor.set(1.0f, 0.0f, 1.0f);

	//Connection colors do not need to be stored at present
	positiveConnectionColor.set(1.0f, 0.0f, 0.0f);
	negativeConnectionColor.set(0.0f, 0.0f, 1.0f);

	//Initialize color map
	neuronColorMap = new QHash<unsigned int, RGBColor*>();

	//Default connection mode settings
	connectionMode = 0;
	weightRenderMode = 0;
	singleNeuronID = 0;
	toNeuronID = 0;

	//Default render settings
	fullRenderMode = false;

	//Other defaults
	vertexSize = 7.5;
	sphereRadius = 0.1f;
	sphereQuality = 10;
	drawAxes = true;
	neuronTransparency = 1.0;
}


/*! Destructor */
NetworkDisplay::~NetworkDisplay(){
}


/*----------------------------------------------------------*/
/*-----                  PUBLIC SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Resets the state of the display. */
void NetworkDisplay::networkChanged(){
	//Clear current display information
	connGrpDisplayMap.clear();
	neurGrpDisplayMap.clear();
	clearNeuronColorMap();
	unsetConnectionModeFlag(CONNECTION_MODE_ENABLED);
	singleNeuronID = 0;
	toNeuronID = 0;
	zoomStatus = NO_ZOOM;
	zoomNeuronGroupID = 0;

	if(Globals::networkLoaded()){
		//Make the neuron groups visible by default
		setVisibleNeuronGroupIDs(Globals::getNetwork()->getNeuronGroupIDs(), false);

		//Show connection groups that are smaller than the loading threshold
		QList<unsigned> visibleConGrpIDs;
		QList<ConnectionGroup*> tmpConGrpList = Globals::getNetwork()->getConnectionGroups();
		foreach(ConnectionGroup* tmpConGrp, tmpConGrpList){
			if(tmpConGrp->size() <= connectionLoadingThreshold)
				visibleConGrpIDs.append(tmpConGrp->getID());
		}
		setVisibleConnectionGroupIDs(visibleConGrpIDs, false);
	}
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Clears the zoom status. This should be done automatically whenever
	the 3D network perspective is altered by the user. */
void NetworkDisplay::clearZoom(){
	int oldZoomStatus = zoomStatus;
	zoomStatus = NO_ZOOM;

	//Inform other classs about the change
	if(oldZoomStatus != NO_ZOOM)
		emit networkDisplayChanged();
}


/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Adds a default colour, which will not be erased when the colour map is cleared.
	The invoking class is responsible for deleting this colour. */
void NetworkDisplay::addDefaultColor(RGBColor *color){
	if(defaultColorMap.contains(color))
		throw SpikeStreamException("Default colour has already been set.");
	defaultColorMap[color] = true;
}


/*! Adds highlight neurons. If the color is already set it is deleted and replaced with the
	new color.  */
void NetworkDisplay::addHighlightNeurons(const QList<unsigned int>& neuronIDs, RGBColor* color){
	//Obtain and lock the mutex
	QMutexLocker locker(&mutex);

	foreach(unsigned int neurID, neuronIDs){
		//Add color if it is not already in the map
		if(!neuronColorMap->contains(neurID)){
			(*neuronColorMap)[neurID] = color;
		}
		//Replace the color
		else{
			//Delete the color associated with the neuron id if it is not a default color
			if(!defaultColorMap.contains( (*neuronColorMap)[neurID]) )
				delete (*neuronColorMap)[neurID];
			//Set the color in the map
			(*neuronColorMap)[neurID] = color;
		}
	}

	//Inform other classes that the display has changed
	emit networkDisplayChanged();
}


/*! Adds highlight neurons. If the color is already set it is deleted and replaced with the
	new color.  */
void NetworkDisplay::addHighlightNeurons(const QHash<unsigned int, RGBColor*>& colorMap){
	//Obtain and lock the mutex
	QMutexLocker locker(&mutex);

	for( QHash<unsigned int, RGBColor*>::const_iterator iter = colorMap.begin(); iter != colorMap.end(); ++iter){
		//Add color if it is not already in the map
		if(!neuronColorMap->contains(iter.key())){
			(*neuronColorMap)[iter.key()] = iter.value();
		}
		//Replace the color
		else{
			//Delete the color associated with the neuron id if it is not a default color
			if(!defaultColorMap.contains( (*neuronColorMap)[iter.key()]) )
				delete (*neuronColorMap)[iter.key()];
			//Set the color in the map
			(*neuronColorMap)[iter.key()] = iter.value();
		}
	}

	//Inform other classes that the display has changed
	emit networkDisplayChanged();
}


/*! Removes highlight neurons */
void NetworkDisplay::removeHighlightNeurons(const QList<unsigned int>& neuronIDs){
	foreach(unsigned int neurID, neuronIDs){
		if(neuronColorMap->contains(neurID)){
			//Delete the color associated with the neuron id if it is not a default color
			if(!defaultColorMap.contains( (*neuronColorMap)[neurID]) )
				delete (*neuronColorMap)[neurID];

			//Remove the entry for the neuron
			neuronColorMap->remove(neurID);
		}
	}

	//Inform other classes that the display has changed
	emit networkDisplayChanged();
}


/*! Returns true if the specified connection group is currently visible */
bool NetworkDisplay::connectionGroupVisible(unsigned int conGrpID){
	if(connGrpDisplayMap.contains(conGrpID))
		return true;
	return false;
}


/*! Returns true if the specified neuron group is currently visible */
bool NetworkDisplay::neuronGroupVisible(unsigned int neurGrpID){
	if(neurGrpDisplayMap.contains(neurGrpID))
		return true;
	return false;
}


/*! Loads the network display settings from the configuration file.
	Throws an exception if the parameters are not present in the file. */
void NetworkDisplay::loadDisplaySettings(ConfigLoader* configLoader){
	vertexSize = Util::getFloat( configLoader->getParameter("vertex_size") );
	drawAxes = Util::getBool( configLoader->getParameter("draw_axes") );
	sphereRadius = Util::getFloat( configLoader->getParameter("sphere_radius") );
	sphereQuality = Util::getUInt( configLoader->getParameter("sphere_quality") );
	connectionLoadingThreshold = Util::getInt( configLoader->getParameter("connection_loading_threshold") );
	minimumConnectionRadius = Util::getFloat( configLoader->getParameter("minimum_connection_radius") );
	weightRadiusFactor = Util::getFloat( configLoader->getParameter("weight_radius_factor") );
	connectionQuality = Util::getUInt( configLoader->getParameter("connection_quality") );
}


/*! Locks the mutex associated with this class */
void NetworkDisplay::lockMutex(){
	mutex.lock();
}


/*! Sets the visibility of the specified connection group */
void NetworkDisplay::setConnectionGroupVisibility(unsigned int conGrpID, bool visible){
	if(visible){
		connGrpDisplayMap[conGrpID] = true;
	}
	else{
		connGrpDisplayMap.remove(conGrpID);
	}

	//Inform other classes that the display has changed
	emit networkDisplayChanged();
}


/*! Sets the render mode of the 3D display */
void NetworkDisplay::setFullRenderMode(bool fullRenderMode){
	this->fullRenderMode = fullRenderMode;
	emit networkDisplayChanged();
}


/*! Sets the map containing specified neuron colours */
void NetworkDisplay::setNeuronColorMap(QHash<unsigned int, RGBColor*>* newMap){
	//Obtain and lock the mutex
	QMutexLocker locker(&mutex);

	//Clear current entries and delete existing map
	clearNeuronColorMap();
	delete neuronColorMap;

	//Point map to new map
	neuronColorMap = newMap;

	//Inform other classes that the display has changed
	emit neuronGroupDisplayChanged();
}


/*! Sets the visibility of the specified neuron group */
void NetworkDisplay::setNeuronGroupVisibility(unsigned int neurGrpID, bool visible){
	if(visible){
		neurGrpDisplayMap[neurGrpID] = true;
	}
	else{
		neurGrpDisplayMap.remove(neurGrpID);
	}
	//Inform other classes that the display has changed
	emit neuronGroupDisplayChanged();
}


/*! Sets the transparency of the neurons */
void NetworkDisplay::setNeuronTransparency(float neuronTransparency){
	 this->neuronTransparency = neuronTransparency;
	 emit networkDisplayChanged();
}


/*! Sets the list of connection groups that are displayed */
void NetworkDisplay::setVisibleConnectionGroupIDs(const QList<unsigned int>& connGrpIDs, bool emitDisplayChangedSignal){
	//Obtain and lock the mutex
	QMutexLocker locker(&mutex);

	//Add the neuron group IDs to the map
	connGrpDisplayMap.clear();
	for(QList<unsigned int>::ConstIterator iter = connGrpIDs.begin(); iter != connGrpIDs.end(); ++iter)
		connGrpDisplayMap[*iter] = true;

	//Inform other classes that the display has changed
	if(emitDisplayChangedSignal)
		emit networkDisplayChanged();
}


/*! Sets the list of neuron groups that are displayed */
void NetworkDisplay::setVisibleNeuronGroupIDs(const QList<unsigned int>& neurGrpIDs, bool emitDisplayChangedSignal){
	//Obtain and lock the mutex
	QMutexLocker locker(&mutex);

	neurGrpDisplayMap.clear();
	for(QList<unsigned int>::ConstIterator iter = neurGrpIDs.begin(); iter != neurGrpIDs.end(); ++iter)
		neurGrpDisplayMap[*iter] = true;

	//Inform other classes that the display has changed
	if(emitDisplayChangedSignal)
		emit neuronGroupDisplayChanged();
}


/*! Unlocks the mutex associated with this class. */
void NetworkDisplay::unlockMutex(){
	mutex.unlock();
}


/*! Sets the type of connection display - single neuron, from/to, between etc. */
void NetworkDisplay::setConnectionModeFlag(unsigned int flag){
	checkConnectionModeFlag(flag);
	connectionMode |= flag;
}


/*! Clears the type of connection display */
void NetworkDisplay::unsetConnectionModeFlag(unsigned int flag){
	checkConnectionModeFlag(flag);
	//Flip the bits in the flag and then AND it with the connection mode
	connectionMode &= ~flag;
}


/*! Returns true if a specified zoom setting has been applied */
bool NetworkDisplay::isZoomEnabled(){
	if(zoomStatus == NO_ZOOM)
		return false;
	return true;
}


/*! Sets the zoom of the 3D network viewer and emits a signal if
	the zoom settings have changed */
void NetworkDisplay::setZoom(unsigned int neurGrpID, int status){
	int oldStatus = zoomStatus;
	unsigned int oldNeurGrpID = zoomNeuronGroupID;
	zoomStatus = status;
	zoomNeuronGroupID = neurGrpID;

	//Inform other classes if display has changed
	if(oldStatus != zoomStatus|| oldNeurGrpID != neurGrpID)
		emit networkDisplayChanged();
}


/*! Called when the user double clicks on a neuron */
void NetworkDisplay::setSelectedNeuronID(unsigned int id, bool ctrlBtnDown){
	//Connection mode is disabled and id is valid
	if( !(connectionMode & CONNECTION_MODE_ENABLED) && id != 0){
		setConnectionModeFlag(CONNECTION_MODE_ENABLED);
		singleNeuronID = id;
		toNeuronID = 0;
	}

	//Switch off connection mode if neuron id is invalid
	else if ( (connectionMode & CONNECTION_MODE_ENABLED) && id == 0){
		unsetConnectionModeFlag(CONNECTION_MODE_ENABLED);
		unsetConnectionModeFlag(SHOW_BETWEEN_CONNECTIONS);
		singleNeuronID = 0;
		toNeuronID = 0;
	}

	//Connection mode is enabled for a single neuron
	else if( (connectionMode & CONNECTION_MODE_ENABLED) && !(connectionMode & SHOW_BETWEEN_CONNECTIONS) ){
		//If a different neuron has been double clicked and control button is down
		if(singleNeuronID != id && ctrlBtnDown){
			toNeuronID = id;
			setConnectionModeFlag(SHOW_BETWEEN_CONNECTIONS);
			return;
		}
		//Control button is not down - select a different neuron
		else if(singleNeuronID != id && !ctrlBtnDown){
			singleNeuronID = id;
		}
	}

	//In between connection mode
	else if( (connectionMode & CONNECTION_MODE_ENABLED) && (connectionMode & SHOW_BETWEEN_CONNECTIONS) ){
		//A neuron has been double clicked without the control button or the first neuron is also selected as the second neuron
		if( !ctrlBtnDown || singleNeuronID == id ){
			unsetConnectionModeFlag(SHOW_BETWEEN_CONNECTIONS);
			singleNeuronID = id;
			toNeuronID = 0;
		}
		//User has double clicked on a different between neuron
		else if (id != toNeuronID){
			toNeuronID = id;
		}
	}

	emit networkDisplayChanged();
}


/*! Sets display to only show positive connections */
void NetworkDisplay::showPositiveConnections() {
	setConnectionModeFlag(SHOW_POSITIVE_CONNECTIONS);
	unsetConnectionModeFlag(SHOW_NEGATIVE_CONNECTIONS);
	emit networkDisplayChanged();
}


/*! Sets display to only show negative connections */
void NetworkDisplay::showNegativeConnections() {
	setConnectionModeFlag(SHOW_NEGATIVE_CONNECTIONS);
	unsetConnectionModeFlag(SHOW_POSITIVE_CONNECTIONS);
	emit networkDisplayChanged();
}


/*! Sets display to show both positive and negative weights */
void NetworkDisplay::clearWeightFiltering() {
	unsetConnectionModeFlag(SHOW_NEGATIVE_CONNECTIONS);
	unsetConnectionModeFlag(SHOW_POSITIVE_CONNECTIONS);
	emit networkDisplayChanged();
}


/*! Sets display to only show from connections */
void NetworkDisplay::showFromConnections() {
	setConnectionModeFlag(SHOW_FROM_CONNECTIONS);
	unsetConnectionModeFlag(SHOW_TO_CONNECTIONS);
	emit networkDisplayChanged();
}


/*! Sets display to only show to connections */
void NetworkDisplay::showToConnections() {
	setConnectionModeFlag(SHOW_TO_CONNECTIONS);
	unsetConnectionModeFlag(SHOW_FROM_CONNECTIONS);
	emit networkDisplayChanged();
}


/*! Sets the display to show both positive and negative connections */
void NetworkDisplay::clearDirectionFiltering(){
	unsetConnectionModeFlag(SHOW_FROM_CONNECTIONS);
	unsetConnectionModeFlag(SHOW_TO_CONNECTIONS);
	emit networkDisplayChanged();
}


/*! Weights are rendered as lines, not polygons */
void  NetworkDisplay::disableWeightRender(){
	unsetWeightRenderFlag(WEIGHT_RENDER_ENABLED);
	emit networkDisplayChanged();
}


/*! Renders the temporary values of weights in the database using polygons. */
void  NetworkDisplay::renderTempWeights(){
	setWeightRenderFlag(WEIGHT_RENDER_ENABLED);
	setWeightRenderFlag(RENDER_TEMP_WEIGHTS);
	unsetWeightRenderFlag(RENDER_CURRENT_WEIGHTS);
	emit networkDisplayChanged();
}


/*! Renders the current values of weights in the database using polygons. */
void  NetworkDisplay::renderCurrentWeights(){
	setWeightRenderFlag(WEIGHT_RENDER_ENABLED);
	setWeightRenderFlag(RENDER_CURRENT_WEIGHTS);
	unsetWeightRenderFlag(RENDER_TEMP_WEIGHTS);
	emit networkDisplayChanged();
}


/*----------------------------------------------------------*/
/*-----               PRIVATE METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Clears all of the entries in the neuron color map */
void NetworkDisplay::clearNeuronColorMap(){
	QHash<unsigned int, RGBColor*>::iterator colorMapEnd = neuronColorMap->end();
	for(QHash<unsigned int, RGBColor*>::iterator iter = neuronColorMap->begin(); iter != colorMapEnd; ++iter){
		//Don't delete one of the default colours - several neuron ids can point to the same default color for efficiency
		if(!defaultColorMap.contains(iter.value()))
			delete iter.value();
	}
	neuronColorMap->clear();
}


/*! Checks that a particular connection mode flag is valid and throws an exception if not */
void NetworkDisplay::checkConnectionModeFlag(unsigned int flag){
	if(flag == CONNECTION_MODE_ENABLED)
		return;
	if(flag == SHOW_BETWEEN_CONNECTIONS)
		return;
	if(flag == SHOW_POSITIVE_CONNECTIONS)
		return;
	if(flag == SHOW_NEGATIVE_CONNECTIONS)
		return;
	if(flag == SHOW_FROM_CONNECTIONS)
		return;
	if(flag == SHOW_TO_CONNECTIONS)
		return;
	throw SpikeStreamException("Connection mode flag not recognized: " + QString::number(flag));
}


/*! Checks that a particular weight render flag is valid and throws an exception if not */
void NetworkDisplay::checkWeightRenderFlag(unsigned int flag){
	if(flag == WEIGHT_RENDER_ENABLED)
		return;
	if(flag == RENDER_TEMP_WEIGHTS)
		return;
	if(flag == RENDER_CURRENT_WEIGHTS)
		return;
	throw SpikeStreamException("Weight render flag not recognized: " + QString::number(flag));
}


/*! Sets a flag controlling weight rendering */
void NetworkDisplay::setWeightRenderFlag(unsigned flag){
	checkWeightRenderFlag(flag);
	weightRenderMode |= flag;
}


/*! Unsets a flag controlling weight rendering */
void NetworkDisplay::unsetWeightRenderFlag(unsigned flag){
	checkWeightRenderFlag(flag);
	//Flip the bits in the flag and then AND it with the weight render mode
	weightRenderMode &= ~flag;
}

