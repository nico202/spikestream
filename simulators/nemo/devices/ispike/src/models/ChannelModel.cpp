//SpikeStream includes
#include "ChannelModel.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>

/*! Constructor */
ChannelModel::ChannelModel(ISpikeManager* iSpikeManager) : QAbstractTableModel(){
	loadChannels();
}


/*! Destructor */
ChannelModel::~ChannelModel(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int ChannelModel::columnCount(const QModelIndex&) const{
	return NUM_COLS;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
    ready to be displayed in the way requested */
QVariant ChannelModel::data(const QModelIndex & index, int role) const{
    //Return invalid index if index is invalid or no network loaded
    if (!index.isValid())
		return QVariant();

    //Check rows and columns are in range
    if (index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
		return QVariant();

    //Return appropriate data
    if (role == Qt::DisplayRole){
//		if(index.column() == ID_COL)
//			return neurGrpInfoList[index.row()].getID();
//		if(index.column() == NAME_COL)
//			return neurGrpInfoList[index.row()].getName();
//		if(index.column() == DESC_COL)
//			return neurGrpInfoList[index.row()].getDescription();
//		if(index.column() == SIZE_COL)
//			return neurGrpSizeList[index.row()];
//		if(index.column() == NEUR_TYPE_COL)
//			return neurGrpInfoList[index.row()].getNeuronType().getDescription();
    }

	//Icons
    if (role == Qt::DecorationRole){
//		if(index.column() == VIS_COL ){
//			if(Globals::getNetworkDisplay()->neuronGroupVisible(neurGrpInfoList[index.row()].getID()))
//				return QIcon(Globals::getSpikeStreamRoot() + "/images/visible.xpm");
//			return QIcon(Globals::getSpikeStreamRoot() + "/images/hidden.xpm");
//		}
//		if(index.column() == ZOOM_COL){
//			unsigned int tmpNeurGrpID = neurGrpInfoList[index.row()].getID();
//			NetworkDisplay* netDisplay = Globals::getNetworkDisplay();
//			//Zoom enabled on this neuron group
//			if (netDisplay->isZoomEnabled() && netDisplay->getZoomNeuronGroupID() == tmpNeurGrpID){
//				if(netDisplay->getZoomStatus() == NetworkDisplay::ZOOM_SIDE)
//					return QIcon(Globals::getSpikeStreamRoot() + "/images/zoom_to_highlight.xpm");
//				else if(netDisplay->getZoomStatus() == NetworkDisplay::ZOOM_ABOVE)
//					return QIcon(Globals::getSpikeStreamRoot() + "/images/zoom_above_highlight.xpm");
//			}
//			//Zoom is disabled or enabled on a different neuron group
//			return QIcon(Globals::getSpikeStreamRoot() + "/images/zoom_to.xpm");
//		}
//		if(index.column() == PARAM_COL){
//			return QIcon(Globals::getSpikeStreamRoot() + "/images/parameters.xpm");
//		}
	}

    //If we have reached this point ignore request
    return QVariant();
}


/*! Reloads the list of neuron groups */
void ChannelModel::reload(){
	loadChannels();
}


/*! Inherited from QAbstractTableModel. */
bool ChannelModel::setData(const QModelIndex& index, const QVariant&, int) {
	if (!index.isValid())
		return false;
    if (index.row() < 0 || index.row() >= rowCount())
		return false;

//    //Change visibility of neuron group
//	if(index.column() == VIS_COL){
//		unsigned int tmpNeurGrpID = neurGrpInfoList[index.row()].getID();
//		if(Globals::getNetworkDisplay()->neuronGroupVisible(tmpNeurGrpID))
//			Globals::getNetworkDisplay()->setNeuronGroupVisibility(tmpNeurGrpID, false);
//		else
//			Globals::getNetworkDisplay()->setNeuronGroupVisibility(tmpNeurGrpID, true);

//		//Emit signal that data has changed and return true to indicate data set succesfully.
//		emit dataChanged(index, index);
//		return true;
//    }

//    //Change zoom of neuron group
//	if(index.column() == ZOOM_COL){
//		unsigned int tmpNeurGrpID = neurGrpInfoList[index.row()].getID();
//		NetworkDisplay* netDisplay = Globals::getNetworkDisplay();
//		//We are already zoomed on this neuron
//		if (netDisplay->isZoomEnabled() && netDisplay->getZoomNeuronGroupID() == tmpNeurGrpID){
//			if(netDisplay->getZoomStatus() == NetworkDisplay::ZOOM_SIDE)//We are zoomed into neuron group from the side
//				netDisplay->setZoom(tmpNeurGrpID, NetworkDisplay::ZOOM_ABOVE);//Zoom neuron group from above
//			else if(netDisplay->getZoomStatus() == NetworkDisplay::ZOOM_ABOVE)//We are zoomed into neuron group from above
//				netDisplay->setZoom(0, NetworkDisplay::ZOOM_SIDE);//Zoom whole network from side
//		}

//		//Zoom is off or set for another neuron group
//		else {
//			netDisplay->setZoom(tmpNeurGrpID, NetworkDisplay::ZOOM_SIDE);//Zoom neuron group from side
//		}

//		//Call reset because any of the rows could have changed
//		reset();
//		return true;
//    }


    //If we have reached this point no data has been set
    return false;
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant ChannelModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
		return QVariant();

    if (orientation == Qt::Horizontal){
		if(section == CHANNEL_NAME_COL)
			return "Channel";
		if(section == NEURON_GROUP_NAME_COL)
			return "Neuron Group";
		if(section == PARAM_COL)
			return "Parameters";
		if(section == DELETE_COL)
			return "Delete";
    }

	return QVariant();

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int ChannelModel::rowCount(const QModelIndex&) const{
	return 0;//FIXME
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Loads the current list of connected channels from the ISpikeManager */
void ChannelModel::loadChannels(){


    //Instruct listening classes to reload data
    this->reset();
}


