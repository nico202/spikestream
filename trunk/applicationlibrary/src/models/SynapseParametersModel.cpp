//SpikeStream includes
#include "Globals.h"
#include "NetworkDisplay.h"
#include "SynapseParametersModel.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>


/*! Constructor */
SynapseParametersModel::SynapseParametersModel(unsigned int synapseTypeID){
	//Listen for changes in the network
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));

	//Store the synapse type - this class can only handle connections of a single type
	this->synapseTypeID = synapseTypeID;

	//Load up the parameters for the current network
	loadParameters();
}


/*! Destructor */
SynapseParametersModel::~SynapseParametersModel(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int SynapseParametersModel::columnCount(const QModelIndex&) const{
	return parameterInfoList.size() + 2;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
	ready to be displayed in the way requested */
QVariant SynapseParametersModel::data(const QModelIndex & index, int role) const{
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
			return conGrpInfoList[index.row()].getDescription().left(30) + "(" + QString::number(conGrpInfoList[index.row()].getID()) + ")";
		if(index.column() > 0 && index.column() <= parameterInfoList.size()){
			unsigned int conGrpID = conGrpInfoList[index.row()].getID();

			//Check that we have the parameter data
			if(parameterMap.contains(conGrpID) && parameterMap[conGrpID].contains(parameterInfoList.at(index.column()-1).getName())){
				ParameterInfo info = parameterInfoList.at(index.column()-1);

				//Only return double value if parameter is a double
				if(info.getType() == ParameterInfo::DOUBLE){
					return parameterMap[conGrpID][info.getName()];
				}
				else if(info.getType() == ParameterInfo::BOOLEAN){
					if(parameterMap[conGrpID][info.getName()] == 0)
						return "off";
					return "on";
				}
				else
					throw SpikeStreamException("Parameter type not recognized: " + QString::number(info.getType()));

			}
			//Cannot find parameter data - error has occurred somewhere
			else{
				throw SpikeStreamException("Maps missing entry somewhere.");
			}
		}
	}

	//Icons
	if (role == Qt::DecorationRole){
		//Edit button
		if(index.column() == parameterInfoList.size() + 1 ){
			QIcon tmpIcon(Globals::getSpikeStreamRoot() + "/images/edit.png");
			return tmpIcon;
		}
	}

	//Size hint
	if(role == Qt::SizeHintRole){
		//Edit button
		if(index.column() == parameterInfoList.size() + 1 ){
			return QSize(40, 40);
		}
	}

	//If we have reached this point ignore request
	return QVariant();
}


/*! Returns the connection group info associated with a particular row */
ConnectionGroupInfo SynapseParametersModel::getConnectionGroupInfo(int row){
	if(row >= conGrpInfoList.size())
		throw SpikeStreamException("Request for connection group info at row " + QString::number(row) + " is out of range.");
	return conGrpInfoList.at(row);
}


/*! Returns a list of information about the parameters */
QList<ParameterInfo> SynapseParametersModel::getParameterInfoList(){
	return parameterInfoList;
}


/*! Returns the parameter values associated with a connection group in a specific row. */
QHash<QString, double> SynapseParametersModel::getParameterValues(int row){
	ConnectionGroupInfo info = getConnectionGroupInfo(row);
	if(!parameterMap.contains(info.getID()))
		throw SpikeStreamException("Mismatch between parameter map and connection group info list.");

	return parameterMap[info.getID()];
}


/*! Inherited from QAbstractTableModel */
bool SynapseParametersModel::setData(const QModelIndex& index, const QVariant&, int) {
	if (!index.isValid() || !Globals::networkLoaded())
		return false;
	if (index.row() < 0 || index.row() >= rowCount())
		return false;

	//If we have reached this point no data has been set
	return false;
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant SynapseParametersModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal){
		if(section == 0)
			return "Connection Group";
		if(section == parameterInfoList.size() + 1)//Edit column
			return "";
		if(section > 0 && section <= parameterInfoList.size())//Parameter name column
			return parameterInfoList.at(section - 1).getName();
	}

	return QVariant();
}

/*! Reloads the parameters from the database */
void SynapseParametersModel::reload(){
	loadParameters();
}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int SynapseParametersModel::rowCount(const QModelIndex&) const{
	return conGrpInfoList.size();
}


/*--------------------------------------------------------*/
/*-------              PRIVATE SLOTS               -------*/
/*--------------------------------------------------------*/

/*! Slot called when the network changes */
void SynapseParametersModel::networkChanged(){
	loadParameters();
}


/*--------------------------------------------------------*/
/*-------             PRIVATE METHODS              -------*/
/*--------------------------------------------------------*/

/*! Runs a check on the loaded parameters to make sure that everything matches up */
void SynapseParametersModel::checkParameters(){
	foreach(ConnectionGroupInfo conGrpInfo, conGrpInfoList){
		if(!parameterMap.contains(conGrpInfo.getID()))
			throw SpikeStreamException("Parameter map does not contain a loaded connection group");

		//Check that keys in parameter map match keys for this synapse type
		QHash<QString, double> conGrpParamMap = parameterMap[conGrpInfo.getID()];
		QList<QString> paramKeys =conGrpParamMap.keys();
		foreach(QString key, paramKeys){
			bool paramFound = false;
			foreach(ParameterInfo paramInfo, parameterInfoList){
				if(paramInfo.getName() == key)
					paramFound = true;
			}
			if(!paramFound){
				throw SpikeStreamException("Key " + key + " from connection group not found in parameter info list");
			}
		}
	}
}


/*! Loads the current list of connection groups if a network is present */
void SynapseParametersModel::loadParameters(){
	//Clear lists of information
	conGrpInfoList.clear();
	parameterMap.clear();
	parameterInfoList.clear();

	//Do nothing if network is not loaded
	if(!Globals::networkLoaded()){
		this->reset();
		return;
	}

	try{
		//Get list of current connection group info filtered by the synapse type
		conGrpInfoList = Globals::getNetwork()->getConnectionGroupsInfo(synapseTypeID);

		//Check that they are all the same type
		bool firstTime = true;
		foreach(ConnectionGroupInfo info, conGrpInfoList){
			if(firstTime){
				synapseTypeID = info.getSynapseTypeID();
				firstTime = false;
			}
			else if(synapseTypeID != info.getSynapseTypeID()){
				throw SpikeStreamException("SynapseParametersModel only supports synapses of a single type.");
			}
		}

		//Get list of available parameters
		parameterInfoList = Globals::getNetworkDao()->getSynapseType(synapseTypeID).getParameterInfoList();

		//Get the parameters for each connection group
		foreach(ConnectionGroupInfo conGrpInfo, conGrpInfoList){
			parameterMap[conGrpInfo.getID()] = Globals::getNetwork()->getConnectionGroup(conGrpInfo.getID())->getParameters();
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


