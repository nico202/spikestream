#include "Globals.h"
#include "GlobalVariables.h"
#include "NetworkDisplay.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Constructor */
NetworkDisplay::NetworkDisplay(){
    //Reset this class when the network is changed
    connect(Globals::getEventRouter(), SIGNAL (networkChangedSignal()), this, SLOT(networkChanged()), Qt::QueuedConnection);

    //Inform other classes when the display has changed
    connect(this, SIGNAL(networkDisplayChanged()), Globals::getEventRouter(), SLOT(networkDisplayChangedSlot()), Qt:: QueuedConnection);

    //Listen for changes to network viewer
    connect(Globals::getEventRouter(), SIGNAL(networkViewChangedSignal()), this, SLOT(clearZoom()));

    //Set the default colors and store addresses in a map to prevent deletion
    defaultNeuronColor.set(0.0f, 0.0f, 0.0f);
    defaultColorMap[&defaultNeuronColor] = true;
    singleNeuronColor.set(0.0f, 1.0f, 0.0f);
    negativeConnectionColor.set(0.0f, 0.0f, 1.0f);
    defaultColorMap[&negativeConnectionColor] = true;
    positiveConnectionColor.set(1.0f, 0.0f, 0.0f);
    defaultColorMap[&positiveConnectionColor] = true;
    firingNeuronColor.set(1.0f, 0.0f, 1.0f);
    defaultColorMap[&firingNeuronColor] = true;

    //Initialize color map
    neuronColorMap = new QHash<unsigned int, RGBColor*>();

    //Default connection mode settings
    connectionMode = 0;
    setConnectionModeFlag(SHOW_POSITIVE_CONNECTIONS);
    setConnectionModeFlag(SHOW_NEGATIVE_CONNECTIONS);
    setConnectionModeFlag(SHOW_ALL_NEURON_CONNECTIONS);
    singleNeuronID = 0;
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

    //Make the neuron groups visible by default
    if(Globals::networkLoaded()){
	setVisibleNeuronGroupIDs(Globals::getNetwork()->getNeuronGroupIDs());
	setVisibleConnectionGroupIDs(Globals::getNetwork()->getConnectionGroupIDs());
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

/*! Clears all of the entries in the neuron color map */
void NetworkDisplay::clearNeuronColorMap(){
    QHash<unsigned int, RGBColor*>::iterator colorMapEnd = neuronColorMap->end();
    for(QHash<unsigned int, RGBColor*>::iterator iter = neuronColorMap->begin(); iter != colorMapEnd; ++iter){
	//Don't delete one of the default colours - several neuron ids can point to the same default color for efficiency
	if(!defaultColorMap.contains(iter.value()))
	    delete iter.value();
    }
    neuronColorMap->clear();

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
    emit networkDisplayChanged();
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
    emit networkDisplayChanged();
}


/*! Sets the list of connection groups that are displayed */
void NetworkDisplay::setVisibleConnectionGroupIDs(const QList<unsigned int>& connGrpIDs){
    //Obtain and lock the mutex
    QMutexLocker locker(&mutex);

    //Add the neuron group IDs to the map
    connGrpDisplayMap.clear();
    for(QList<unsigned int>::ConstIterator iter = connGrpIDs.begin(); iter != connGrpIDs.end(); ++iter)
	connGrpDisplayMap[*iter] = true;

    //Inform other classes that the display has changed
    emit networkDisplayChanged();
}


/*! Sets the list of neuron groups that are displayed */
void NetworkDisplay::setVisibleNeuronGroupIDs(const QList<unsigned int>& neurGrpIDs){
    //Obtain and lock the mutex
    QMutexLocker locker(&mutex);

    neurGrpDisplayMap.clear();
    for(QList<unsigned int>::ConstIterator iter = neurGrpIDs.begin(); iter != neurGrpIDs.end(); ++iter)
	neurGrpDisplayMap[*iter] = true;

    //Inform other classes that the display has changed
    emit networkDisplayChanged();
}


void NetworkDisplay::unlockMutex(){
    mutex.unlock();
}


void NetworkDisplay::setConnectionModeFlag(unsigned int flag){
    checkConnectionModeFlag(flag);
    connectionMode |= flag;
}


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

FIXME!!

void NetworkDisplay::setSingleNeuronID(unsigned int id){
    this->singleNeuronID = id;
    //Switch off connection mode if neuron id is invalid
    if(id == 0){
	unsetConnectionModeFlag(CONNECTION_MODE_ENABLED);
	unsetConnectionModeFlag(SHOW_SINGLE_NEURON_CONNECTIONS);
    }
    //Turn on connection mode
    else{
	setConnectionModeFlag(SHOW_SINGLE_NEURON_CONNECTIONS);
    }

    emit networkDisplayChanged();
}


void NetworkDisplay::setBetweenFromNeuronID(unsigned int id){
    this->betweenFromNeuronID = id;
    emit networkDisplayChanged();
}

void NetworkDisplay::setBetweenToNeuronID(unsigned int id){
    this->betweenToNeuronID = id;
    emit networkDisplayChanged();
}


/*----------------------------------------------------------*/
/*-----               PRIVATE METHODS                  -----*/
/*----------------------------------------------------------*/

void NetworkDisplay::checkConnectionModeFlag(unsigned int flag){
    if(flag == SHOW_SINGLE_NEURON_CONNECTIONS)
	return;
    if(flag == SHOW_BETWEEN_NEURON_CONNECTIONS)
	return;
    if(flag == SHOW_POSITIVE_CONNECTIONS)
	return;
    if(flag == SHOW_NEGATIVE_CONNECTIONS)
	return;
    if(flag == SHOW_FROM_NEURON_CONNECTIONS)
	return;
    if(flag == SHOW_TO_NEURON_CONNECTIONS)
	return;
    if(flag == SHOW_ALL_NEURON_CONNECTIONS)
	return;
    throw SpikeStreamException("Connection mode flag not recognized: " + QString::number(flag));
}

