//SpikeStream includes
#include "Globals.h"
#include "GlobalVariables.h"
#include "NetworkDisplay.h"
#include "NetworkDao.h"
#include "ConnectionsModel.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>


/*! Constructor */
ConnectionsModel::ConnectionsModel() : QAbstractTableModel(){
	connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(networkDisplayChanged()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(weightsChangedSignal()), this, SLOT(networkDisplayChanged()), Qt::QueuedConnection);
}


/*! Destructor */
ConnectionsModel::~ConnectionsModel(){
    clearConnectionsList();
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int ConnectionsModel::columnCount(const QModelIndex&) const{
    return numCols;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
    ready to be displayed in the way requested */
QVariant ConnectionsModel::data(const QModelIndex & index, int role) const{
    //Return invalid index if index is invalid or no network loaded
    if (!index.isValid())
		return QVariant();
    if(!Globals::networkLoaded())
		return QVariant();

    //Check rows and columns are in range
    if (index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
		return QVariant();

    //Return appropriate data
    if (role == Qt::DisplayRole){
		//Get pointer to the appropriate Connection class
		Connection* tmpConnection = connectionsList[index.row()];

		if(index.column() == idCol)
			return tmpConnection->getID();
		if(index.column() == fromIDCol)
			return tmpConnection->getFromNeuronID();
		if(index.column() == toIDCol)
			return tmpConnection->getToNeuronID();
		if(index.column() == delayCol)
			return tmpConnection->getDelay();
		if(index.column() == weightCol)
			return tmpConnection->getWeight();
		if(index.column() == tmpWeightCol)
			return tmpConnection->getTempWeight();
    }


    //If we have reached this point ignore request
    return QVariant();
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant ConnectionsModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
		return QVariant();

    if (orientation == Qt::Horizontal){
		if(section == idCol)
			return "ID";
		if(section == fromIDCol)
			return "From";
		if(section == toIDCol)
			return "To";
		if(section == delayCol)
			return "Delay";
		if(section == weightCol)
			return "Weight";
		if(section == tmpWeightCol)
			return "Temp Weight";
    }

    return QVariant();

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int ConnectionsModel::rowCount(const QModelIndex&) const{
    return connectionsList.size();
}


/*! Instructs viewers to reload model when network display has changed */
void ConnectionsModel::networkDisplayChanged(){
	//Empty connections list
	//clearConnectionsList();
	connectionsList.clear();

	//Fill connections list if appropriate
	NetworkDisplay* netDisplay = Globals::getNetworkDisplay();
	unsigned int connectionMode = netDisplay->getConnectionMode();
	if(connectionMode & CONNECTION_MODE_ENABLED){
		unsigned int singleNeuronID = netDisplay->getSingleNeuronID();
		unsigned int toNeuronID = netDisplay->getToNeuronID();
		connectionsList = Globals::getNetwork()->getConnections(connectionMode, singleNeuronID, toNeuronID);
	}
	reset();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Empties the connection list and deletes the Connection classes */
void ConnectionsModel::clearConnectionsList(){
    foreach(Connection* tmpCon, connectionsList)
		delete tmpCon;
    connectionsList.clear();
}


