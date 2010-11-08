//SpikeStream includes
#include "Globals.h"
#include "NetworkDisplay.h"
#include "NeuronGroupModel.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>

/*! Constructor */
NeuronGroupModel::NeuronGroupModel() : QAbstractTableModel(){
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(loadNeuronGroups()));
    connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(networkDisplayChanged()));
}


/*! Destructor */
NeuronGroupModel::~NeuronGroupModel(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Clears the list of selected neuron groups */
void NeuronGroupModel::clearSelection(){
	selectionMap.clear();
}


/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int NeuronGroupModel::columnCount(const QModelIndex&) const{
	return NUM_COLS;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
    ready to be displayed in the way requested */
QVariant NeuronGroupModel::data(const QModelIndex & index, int role) const{
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
		if(index.column() == ID_COL)
			return neurGrpInfoList[index.row()].getID();
		if(index.column() == NAME_COL)
			return neurGrpInfoList[index.row()].getName();
		if(index.column() == DESC_COL)
			return neurGrpInfoList[index.row()].getDescription();
		if(index.column() == SIZE_COL)
			return neurGrpSizeList[index.row()];
		if(index.column() == NEUR_TYPE_COL)
			return neurGrpInfoList[index.row()].getNeuronTypeID();
    }

	//Icons
    if (role == Qt::DecorationRole){
		if(index.column() == VIS_COL ){
			if(Globals::getNetworkDisplay()->neuronGroupVisible(neurGrpInfoList[index.row()].getID()))
				return QIcon(Globals::getSpikeStreamRoot() + "/images/visible.xpm");
			return QIcon(Globals::getSpikeStreamRoot() + "/images/hidden.xpm");
		}
		if(index.column() == ZOOM_COL){
			unsigned int tmpNeurGrpID = neurGrpInfoList[index.row()].getID();
			NetworkDisplay* netDisplay = Globals::getNetworkDisplay();
			//Zoom enabled on this neuron group
			if (netDisplay->isZoomEnabled() && netDisplay->getZoomNeuronGroupID() == tmpNeurGrpID){
				if(netDisplay->getZoomStatus() == NetworkDisplay::ZOOM_SIDE)
					return QIcon(Globals::getSpikeStreamRoot() + "/images/zoom_to_highlight.xpm");
				else if(netDisplay->getZoomStatus() == NetworkDisplay::ZOOM_ABOVE)
					return QIcon(Globals::getSpikeStreamRoot() + "/images/zoom_above_highlight.xpm");
			}
			//Zoom is disabled or enabled on a different neuron group
			return QIcon(Globals::getSpikeStreamRoot() + "/images/zoom_to.xpm");
		}
		if(index.column() == PARAM_COL){
			return QIcon(Globals::getSpikeStreamRoot() + "/images/parameters.xpm");
		}
	}

	//Check boxes
	if(role == Qt::CheckStateRole){
		if(index.column() == SELECT_COL){
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


/*! Returns the neuron group info associated with the specified index.
	Throws an exception if the index cannot be found. */
NeuronGroupInfo NeuronGroupModel::getInfo(const QModelIndex& index) const{
	if(index.row() >= neurGrpInfoList.size())
		throw SpikeStreamException("Index out of range: " + QString::number(index.row()));
	return neurGrpInfoList[index.row()];
}


/*! Returns the parameters associated with a neuron group */
QHash<QString, double> NeuronGroupModel::getParameters(int row){
	if(row >= rowCount())
		throw SpikeStreamException("Failed to get parameters: row out of range: " + QString::number(row));
	return neurGrpInfoList[row].getParameterMap();
}


/*! Returns the IDs of the selected neuron groups */
QList<unsigned int> NeuronGroupModel::getSelectedNeuronGroupIDs(){
	//Double check lengths
	if(neurGrpInfoList.size() < selectionMap.size())
		throw SpikeStreamException("There are more selected indexes than indexes");

	QList<unsigned int> neurGrpIDList;
	foreach(unsigned int index, selectionMap.keys()){
		neurGrpIDList.append(neurGrpInfoList.at(index).getID());
	}

	//Return list
	return neurGrpIDList;
}


/*! Reloads the list of neuron groups */
void NeuronGroupModel::reload(){
	loadNeuronGroups();
}


/*! Selects all or none of the connection groups */
void NeuronGroupModel::selectAllOrNone(){
	//Deselect all groups
	if(selectionMap.size() == neurGrpInfoList.size()){
		selectionMap.clear();
	}

	//Select all groups
	else{
		for(int i=0; i<neurGrpInfoList.size(); ++i)
			selectionMap[i] = true;
	}
	reset();
}


/*! Inherited from QAbstractTableModel. */
bool NeuronGroupModel::setData(const QModelIndex& index, const QVariant&, int) {
    if (!index.isValid() || !Globals::networkLoaded())
		return false;
    if (index.row() < 0 || index.row() >= rowCount())
		return false;

    //Change visibility of neuron group
	if(index.column() == VIS_COL){
		unsigned int tmpNeurGrpID = neurGrpInfoList[index.row()].getID();
		if(Globals::getNetworkDisplay()->neuronGroupVisible(tmpNeurGrpID))
			Globals::getNetworkDisplay()->setNeuronGroupVisibility(tmpNeurGrpID, false);
		else
			Globals::getNetworkDisplay()->setNeuronGroupVisibility(tmpNeurGrpID, true);

		//Emit signal that data has changed and return true to indicate data set succesfully.
		emit dataChanged(index, index);
		return true;
    }

    //Change zoom of neuron group
	if(index.column() == ZOOM_COL){
		unsigned int tmpNeurGrpID = neurGrpInfoList[index.row()].getID();
		NetworkDisplay* netDisplay = Globals::getNetworkDisplay();
		//We are already zoomed on this neuron
		if (netDisplay->isZoomEnabled() && netDisplay->getZoomNeuronGroupID() == tmpNeurGrpID){
			if(netDisplay->getZoomStatus() == NetworkDisplay::ZOOM_SIDE)//We are zoomed into neuron group from the side
				netDisplay->setZoom(tmpNeurGrpID, NetworkDisplay::ZOOM_ABOVE);//Zoom neuron group from above
			else if(netDisplay->getZoomStatus() == NetworkDisplay::ZOOM_ABOVE)//We are zoomed into neuron group from above
				netDisplay->setZoom(0, NetworkDisplay::ZOOM_SIDE);//Zoom whole network from side
		}

		//Zoom is off or set for another neuron group
		else {
			netDisplay->setZoom(tmpNeurGrpID, NetworkDisplay::ZOOM_SIDE);//Zoom neuron group from side
		}

		//Call reset because any of the rows could have changed
		reset();
		return true;
    }

	//Change selection status of neuron group
	if(index.column() == SELECT_COL){
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
QVariant NeuronGroupModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
		return QVariant();

    if (orientation == Qt::Horizontal){
		if(section == ID_COL)
			return "ID";
		if(section == NAME_COL)
			return "Name";
		if(section == DESC_COL)
			return "Description";
		if(section == SIZE_COL)
			return "Size";
		if(section == NEUR_TYPE_COL)
			return "Neuron Type";
		if(section == PARAM_COL)
			return "Parameters";
    }

	return QVariant();

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int NeuronGroupModel::rowCount(const QModelIndex&) const{
    return neurGrpInfoList.size();
}


/*! Loads the current list of neuron groups if a network is present */
void NeuronGroupModel::loadNeuronGroups(){
    //Clear current list of neuron group information
    neurGrpInfoList.clear();
	neurGrpSizeList.clear();

    //Get list of current neuron group info
    if(Globals::networkLoaded())
		neurGrpInfoList = Globals::getNetwork()->getNeuronGroupsInfo();

	//Load up sizes of connection groups
	for(int i=0; i<neurGrpInfoList.size(); ++i)
		neurGrpSizeList.append( Globals::getNetworkDao()->getNeuronCount( neurGrpInfoList.at(i) ) );

	//Sanity check
	if(neurGrpInfoList.size() != neurGrpSizeList.size())
		qCritical()<<"Mismatch between list of neuron group info and list of neuron group size.";

    //Instruct listening classes to reload data
    this->reset();
}


/*! Instructs viewers to reload model when network display has changed */
void NeuronGroupModel::networkDisplayChanged(){
    reset();
}


