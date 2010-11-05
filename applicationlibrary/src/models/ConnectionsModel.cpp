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
	connect(Globals::getEventRouter(), SIGNAL(visibleConnectionsChangedSignal()), this, SLOT(visibleConnectionsChanged()), Qt::QueuedConnection);
	connect(Globals::getEventRouter(), SIGNAL(weightsChangedSignal()), this, SLOT(visibleConnectionsChanged()), Qt::QueuedConnection);
}


/*! Destructor */
ConnectionsModel::~ConnectionsModel(){
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
		QList<Connection*>& tmpConList = Globals::getNetworkDisplay()->getVisibleConnectionsList();
		Connection* tmpConnection = tmpConList[index.row()];

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
	return Globals::getNetworkDisplay()->getVisibleConnectionsList().size();
}


/*! Instructs viewers to reload model when network display has changed */
void ConnectionsModel::visibleConnectionsChanged(){
	reset();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/


