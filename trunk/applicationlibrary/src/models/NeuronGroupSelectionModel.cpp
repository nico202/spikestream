//SpikeStream includes
#include "NeuronGroupSelectionModel.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Constructor */
NeuronGroupSelectionModel::NeuronGroupSelectionModel(QList<NeuronGroup*> neuronGroupList) : QAbstractTableModel(){
	this->neurGrpList = neuronGroupList;

	//Select all by default
	for(int i=0; i<neurGrpList.size(); ++i)
		selectionMap[i] = true;
}


/*! Destructor */
NeuronGroupSelectionModel::~NeuronGroupSelectionModel(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int NeuronGroupSelectionModel::columnCount(const QModelIndex&) const{
	return NUM_COLS;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
	ready to be displayed in the way requested */
QVariant NeuronGroupSelectionModel::data(const QModelIndex & index, int role) const{
	//Return invalid index if index is invalid or no network loaded
	if (!index.isValid())
		return QVariant();

	//Check rows and columns are in range
	if (index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
		return QVariant();

	//Return appropriate data
	if (role == Qt::DisplayRole){
		if(index.column() == ID_COL)
			return neurGrpList[index.row()]->getID();
		if(index.column() == NAME_COL)
			return neurGrpList[index.row()]->getInfo().getName();
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


/*! Returns the selected neuron groups */
QList<NeuronGroup*> NeuronGroupSelectionModel::getSelectedNeuronGroups(){
	//Double check lengths
	if(neurGrpList.size() < selectionMap.size())
		throw SpikeStreamException("There are more selected indexes than indexes");

	QList<NeuronGroup*> tmpNeurGrpList;
	foreach(unsigned int index, selectionMap.keys()){
		tmpNeurGrpList.append(neurGrpList.at(index));
	}

	//Return list
	return tmpNeurGrpList;
}


/*! Selects all or none of the connection groups */
void NeuronGroupSelectionModel::selectAllOrNone(){
	//Deselect all groups
	if(selectionMap.size() == neurGrpList.size()){
		selectionMap.clear();
	}

	//Select all groups
	else{
		for(int i=0; i<neurGrpList.size(); ++i)
			selectionMap[i] = true;
	}
	reset();
}



/*! Inherited from QAbstractTableModel. */
bool NeuronGroupSelectionModel::setData(const QModelIndex& index, const QVariant&, int) {
	if (!index.isValid())
		return false;
	if (index.row() < 0 || index.row() >= rowCount())
		return false;

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
QVariant NeuronGroupSelectionModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal){
		if(section == ID_COL)
			return "ID";
		if(section == NAME_COL)
			return "Name";
	}

	return QVariant();
}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int NeuronGroupSelectionModel::rowCount(const QModelIndex&) const{
	return neurGrpList.size();
}



