#include "Globals.h"
#include "NetworkDisplay.h"
using namespace spikestream;


/*! Constructor */
NetworkDisplay::NetworkDisplay(){
    //Reset this class when the network is changed
    connect(Globals::getEventRouter(), SIGNAL (networkChangedSignal()), this, SLOT(networkChanged()), Qt::QueuedConnection);

    //Inform other classes when the display has changed
    connect(this, SIGNAL(networkDisplayChanged()), Globals::getEventRouter(), SLOT(networkDisplayChangedSlot()), Qt:: QueuedConnection);

    //Set the default colors and store addresses in a map to prevent deletion
    defaultNeuronColor.set(0.0f, 0.0f, 0.0f);
    defaultColorMap[&defaultNeuronColor] = true;
    negativeConnectionColor.set(0.0f, 0.0f, 1.0f);
    defaultColorMap[&negativeConnectionColor] = true;
    positiveConnectionColor.set(1.0f, 0.0f, 0.0f);
    defaultColorMap[&positiveConnectionColor] = true;
    firingNeuronColor.set(1.0f, 0.0f, 1.0f);
    defaultColorMap[&firingNeuronColor] = true;

    //Initialize color map
    neuronColorMap = new QHash<unsigned int, RGBColor*>();
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
}


void NetworkDisplay::lockMutex(){
    mutex.lock();
}

void NetworkDisplay::setNeuronColorMap(QHash<unsigned int, RGBColor*>* newMap){
    //Obtain and lock the mutex
    QMutexLocker locker(&mutex);

    //Clear current entries and delete existing map
    clearNeuronColorMap();
    delete neuronColorMap;

    //Point map to new map
    neuronColorMap = newMap;
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


/*----------------------------------------------------------*/
/*-----               PRIVATE METHODS                  -----*/
/*----------------------------------------------------------*/
