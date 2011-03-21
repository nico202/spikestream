//SpikeStream includes
#include "ChannelModel.h"
#include "Globals.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>

/*! Constructor */
ChannelModel::ChannelModel(ISpikeManager* iSpikeManager) : QAbstractTableModel(){
	this->iSpikeManager = iSpikeManager;
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
		if(index.column() == CHANNEL_NAME_COL)
			return channelNameList.at(index.row());
		if(index.column() == NEURON_GROUP_NAME_COL)
			return neuronGroupNameList.at(index.row());
    }

	//Icons
    if (role == Qt::DecorationRole){
		if(index.column() == PARAM_COL ){
			return QIcon(Globals::getSpikeStreamRoot() + "/images/parameters.png");
		}
		if(index.column() == DELETE_COL){
			return QIcon(Globals::getSpikeStreamRoot() + "/images/trash_can.png");
		}
	}

    //If we have reached this point ignore request
    return QVariant();
}


/*! Deletes an input or output channel depending on the row */
void ChannelModel::deleteChannel(int row){
	if(row >= 0 && row < iSpikeManager->getInputChannelCount()){
		iSpikeManager->deleteInputChannel(row);
		reload();
	}
	else if (row - iSpikeManager->getInputChannelCount() < iSpikeManager->getOutputChannelCount()){
		iSpikeManager->deleteOutputChannel(row - iSpikeManager->getInputChannelCount());
		reload();
	}
	else
		throw SpikeStreamException("Failed to delete channel; row out of range: " + QString::number(row));
}


/*! Returns the parameters for a particular active channel */
map<string, Property*> ChannelModel::getParameters(int row){
	if(row >= 0 && row < iSpikeManager->getInputChannelCount())
		return iSpikeManager->getInputParameters(row);
	else if (row - iSpikeManager->getInputChannelCount() < iSpikeManager->getOutputChannelCount())
		return iSpikeManager->getOutputParameters(row - iSpikeManager->getInputChannelCount());
	throw SpikeStreamException("Failed to get parameters; row out of range: " + QString::number(row));
}


/*! Returns true if there are no channels */
bool ChannelModel::isEmpty(){
	if(rowCount() == 0)
		return true;
	return false;
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
    }

	return QVariant();

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int ChannelModel::rowCount(const QModelIndex&) const{
	return channelNameList.size();
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Returns a standard formatted neuron group name including the ID. */
QString ChannelModel::getNeuronGroupName(NeuronGroup* neuronGroup){
	return neuronGroup->getInfo().getName() + " (" + QString::number(neuronGroup->getID()) + ")";
}


/*! Loads the current list of connected channels from the ISpikeManager */
void ChannelModel::loadChannels(){
	channelNameList.clear();
	neuronGroupNameList.clear();

	//Input channels
	QList< QPair<InputChannel*, NeuronGroup*> > inputChannelList = iSpikeManager->getInputChannels();
	for(int i=0; i< inputChannelList.size(); ++i){
		channelNameList.append(inputChannelList[i].first->getChannelDescription().getChannelName().data());
		neuronGroupNameList.append(getNeuronGroupName(inputChannelList[i].second));
	}

	//Output channels
	QList< QPair<OutputChannel*, NeuronGroup*> > outputChannelList = iSpikeManager->getOutputChannels();
	for(int i=0; i< outputChannelList.size(); ++i){
		channelNameList.append(outputChannelList[i].first->getChannelDescription().getChannelName().data());
		neuronGroupNameList.append(getNeuronGroupName(outputChannelList[i].second));
	}

    //Instruct listening classes to reload data
    this->reset();
}


