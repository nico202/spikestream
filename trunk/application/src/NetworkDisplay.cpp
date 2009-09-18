#include "Globals.h"
#include "NetworkDisplay.h"
using namespace spikestream;


/*! Constructor */
NetworkDisplay::NetworkDisplay(){
    //Reset this class when the network is changed
    connect(Globals::getEventRouter(), SIGNAL (networkChangedSignal()), this, SLOT(networkChanged()), Qt::QueuedConnection);

    //Inform other classes when the display has changed
    connect(this, SIGNAL(networkDisplayChanged()), Globals::getEventRouter(), SLOT(networkDisplayChangedSlot()), Qt:: QueuedConnection);

    //Set the default neuron color
    defaultNeuronColor.set(0.0f, 0.0f, 0.0f);
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
    neuronColorMap.clear();

    //Make the neuron groups visible by default
    if(Globals::networkLoaded())
	setVisibleNeuronGroupIDs(Globals::getNetwork()->getNeuronGroupIDs());
}


/*----------------------------------------------------------*/
/*-----               PRIVATE METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Sets the list of connection groups that are displayed */
void NetworkDisplay::setVisibleConnectionGroupIDs(const QList<unsigned int>& connGrpIDs){
    //Add the neuron group IDs to the map
    connGrpDisplayMap.clear();
    for(QList<unsigned int>::ConstIterator iter = connGrpIDs.begin(); iter != connGrpIDs.end(); ++iter)
	connGrpDisplayMap[*iter] = true;

    //Inform other classes that the display has changed
    emit networkDisplayChanged();
}


/*! Sets the list of neuron groups that are displayed */
void NetworkDisplay::setVisibleNeuronGroupIDs(const QList<unsigned int>& neurGrpIDs){
    neurGrpDisplayMap.clear();
    for(QList<unsigned int>::ConstIterator iter = neurGrpIDs.begin(); iter != neurGrpIDs.end(); ++iter)
	neurGrpDisplayMap[*iter] = true;

    //Inform other classes that the display has changed
    emit networkDisplayChanged();
}


