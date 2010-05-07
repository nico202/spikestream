//SpikeStream includes
#include "Globals.h"
#include "NetworkDisplay.h"
#include "ConnectionGroupModel.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>

/*! Constructor */
ConnectionGroupModel::ConnectionGroupModel() : QAbstractTableModel(){
    connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));
}


/*! Destructor */
ConnectionGroupModel::~ConnectionGroupModel(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int ConnectionGroupModel::columnCount(const QModelIndex&) const{
    return numCols;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
    ready to be displayed in the way requested */
QVariant ConnectionGroupModel::data(const QModelIndex & index, int role) const{
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
		if(index.column() == idCol)
			return conGrpInfoList[index.row()].getID();
		if(index.column() == descCol)
			return conGrpInfoList[index.row()].getDescription();
		if(index.column() == fromNeurIDCol)
			return conGrpInfoList[index.row()].getFromNeuronGroupID();
		if(index.column() == toNeurIDCol)
			return conGrpInfoList[index.row()].getToNeuronGroupID();
		if(index.column() == synapseTypeCol)
			return conGrpInfoList[index.row()].getSynapseType();
    }

	//Icons
    if (role == Qt::DecorationRole){
		if(index.column() == visCol ){
			if(Globals::getNetworkDisplay()->connectionGroupVisible(conGrpInfoList[index.row()].getID()))
				return QIcon(Globals::getSpikeStreamRoot() + "/images/visible.xpm");
			return QIcon(Globals::getSpikeStreamRoot() + "/images/hidden.xpm");
		}
    }

	//Check boxes
	if(role == Qt::CheckStateRole){
		if(index.column() == selectCol){
			if(selectionMap.contains(index.row())){
				return true;
			}
			else {
				return false;
			}
		}
	}

    //If we have reached this point ignore request
    return QVariant();
}


/*! Returns the IDs of the selected neuron groups */
QList<unsigned int> ConnectionGroupModel::getSelectedConnectionGroupIDs(){
	//Double check lengths
	if(conGrpInfoList.size() < selectionMap.size())
		throw SpikeStreamException("There are more selected indexes than indexes");

	QList<unsigned int> conGrpIDList;
	foreach(unsigned int index, selectionMap.keys()){
		conGrpIDList.append(conGrpInfoList.at(index).getID());
	}

	//Return list
	return conGrpIDList;
}


bool ConnectionGroupModel::setData(const QModelIndex& index, const QVariant&, int) {
    if (!index.isValid() || !Globals::networkLoaded())
		return false;
    if (index.row() < 0 || index.row() >= rowCount())
		return false;

    //Change visibility of neuron group
    if(index.column() == visCol){
		unsigned int tmpConGrpID = conGrpInfoList[index.row()].getID();
		if(Globals::getNetworkDisplay()->connectionGroupVisible(tmpConGrpID))
			Globals::getNetworkDisplay()->setConnectionGroupVisibility(tmpConGrpID, false);
		else
			Globals::getNetworkDisplay()->setConnectionGroupVisibility(tmpConGrpID, true);

		//Emit signal that data has changed and return true to indicate data set succesfully.
		emit dataChanged(index, index);
		return true;
    }

	//Change selection status of connection group
	if(index.column() == selectCol){
		if(selectionMap.contains(index.row()))
			selectionMap.remove(index.row());
		else
			selectionMap[index.row()] = true;
		reset();
		return true;
	}

    //If we have reached this point no data has been set
    return false;
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant ConnectionGroupModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
		return QVariant();

    if (orientation == Qt::Horizontal){
		if(section == idCol)
			return "ID";
		if(section == descCol)
			return "Description";
		if(section == fromNeurIDCol)
			return "From";
		if(section == toNeurIDCol)
			return "To";
		if(section == synapseTypeCol)
			return "Synapse Type";
    }

    return QVariant();//QString("Roow %1").arg(section);

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int ConnectionGroupModel::rowCount(const QModelIndex&) const{
    return conGrpInfoList.size();
}


/*! Reloads the current list of neuron groups if a network is present */
void ConnectionGroupModel::networkChanged(){
    //Clear current list of neuron group information
    conGrpInfoList.clear();

    //Get list of current neuron group info
    if(Globals::networkLoaded())
		conGrpInfoList = Globals::getNetwork()->getConnectionGroupsInfo();

    //Instruct listening classes to reload data
    this->reset();
}



