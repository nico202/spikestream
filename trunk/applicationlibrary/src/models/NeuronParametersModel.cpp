//SpikeStream includes
#include "Globals.h"
#include "NetworkDisplay.h"
#include "NeuronParametersModel.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>


/*! Constructor */
NeuronParametersModel::NeuronParametersModel(){
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));
	loadParameters();
}


/*! Destructor */
NeuronParametersModel::~NeuronParametersModel(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/


/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int NeuronParametersModel::columnCount(const QModelIndex&) const{
	return parameterInfoList.size() + 2;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
	ready to be displayed in the way requested */
QVariant NeuronParametersModel::data(const QModelIndex & index, int role) const{
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
		if(index.column() == 0)
			return neurGrpInfoList[index.row()].getName() + "(" + QString::number(neurGrpInfoList[index.row()].getID()) + ")";
		if(index.column() > 0 && index.column() <= parameterInfoList.size()){
			unsigned int neurGrpID = neurGrpInfoList[index.row()].getID();
			if(parameterMap.contains(neurGrpID) && parameterMap[neurGrpID].contains(parameterInfoList.at(index.column()-1).getName()))
				return parameterMap[neurGrpID][parameterInfoList.at(index.column()-1).getName()];
			throw SpikeStreamException("Maps missing entry somewhere.");
		}
	}

	//Icons
	if (role == Qt::DecorationRole){
		//Edit button
		if(index.column() == parameterInfoList.size() + 1 ){
			return QIcon(Globals::getSpikeStreamRoot() + "/images/export.png");
		}
	}

	//If we have reached this point ignore request
	return QVariant();
}


/*! Inherited from QAbstractTableModel */
bool NeuronParametersModel::setData(const QModelIndex& index, const QVariant&, int) {
	if (!index.isValid() || !Globals::networkLoaded())
		return false;
	if (index.row() < 0 || index.row() >= rowCount())
		return false;

	//Edit parameters
	if(index.column() == (parameterInfoList.size() - 1) ){
		qDebug()<<"EDITING PARAMETERS";

		//Emit signal that data has changed and return true to indicate data set succesfully.
		emit dataChanged(index, index);
		return true;
	}

	//If we have reached this point no data has been set
	return false;
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant NeuronParametersModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal){
		if(section == 0)
			return "Neuron Group";
		if(section == parameterInfoList.size() + 1)//Edit column
			return "";
		if(section > 0 && section <= parameterInfoList.size())//Parameter name column
			return parameterInfoList.at(section - 1).getName();
	}

	return QVariant();

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int NeuronParametersModel::rowCount(const QModelIndex&) const{
	return neurGrpInfoList.size();
}


/*! Slot called when the network changes */
void NeuronParametersModel::networkChanged(){
	loadParameters();
}


/* Loads the current list of neuron groups if a network is present */
void NeuronParametersModel::loadParameters(){
	//Clear lists of information
	neurGrpInfoList.clear();
	parameterMap.clear();
	parameterInfoList.clear();

	//Do nothing if network is not loaded
	if(!Globals::networkLoaded()){
		this->reset();
		return;
	}

	try{
		//Get list of current neuron group info
		neurGrpInfoList = Globals::getNetwork()->getNeuronGroupsInfo();

		//Check that they are all the same type
		bool firstTime = true;
		foreach(NeuronGroupInfo info, neurGrpInfoList){
			if(firstTime){
				neuronTypeID = info.getNeuronTypeID();
				firstTime = false;
			}
			else if(neuronTypeID != info.getNeuronTypeID()){
				throw SpikeStreamException("NeuronParametersModel only supports neurons of a single type.");
			}
		}

		//Get list of available parameters
		parameterInfoList = Globals::getNetworkDao()->getNeuronType(neuronTypeID).getParameterInfoList();

		//Get the parameters for each neuron group
		foreach(NeuronGroupInfo neurGrpInfo, neurGrpInfoList){
			parameterMap[neurGrpInfo.getID()] = Globals::getNetwork()->getNeuronGroup(neurGrpInfo.getID())->getParameters();
		}

		//Check that it all matches up
		checkParameters();

	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}

	//Instruct listening classes to reload data
	this->reset();
}


/*! Runs a check on the loaded parameters to make sure that everything matches up */
void NeuronParametersModel::checkParameters(){
	foreach(NeuronGroupInfo neurGrpInfo, neurGrpInfoList){
		if(!parameterMap.contains(neurGrpInfo.getID()))
			throw SpikeStreamException("Parameter map does not contain a loaded neuron group");

		//Check that keys in parameter map match keys for this neuron type
		QHash<QString, double> neurGrpParamMap = parameterMap[neurGrpInfo.getID()];
		QList<QString> paramKeys = neurGrpParamMap.keys();
		foreach(QString key, paramKeys){
			bool paramFound = false;
			foreach(ParameterInfo paramInfo, parameterInfoList){
				if(paramInfo.getName() == key)
					paramFound = true;
			}
			if(!paramFound){
				throw SpikeStreamException("Key " + key + " from neuron group not found in parameter info list");
			}
		}
	}
}


