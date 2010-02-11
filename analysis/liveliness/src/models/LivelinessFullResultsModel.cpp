//SpikeStream includes
#include "LivelinessFullResultsModel.h"
#include "Globals.h"
#include "GlobalVariables.h"
#include "RGBColor.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>

/*! Constructor */
LivelinessFullResultsModel::LivelinessFullResultsModel(const AnalysisInfo* analysisInfo, LivelinessDao* livelinessDao) : QAbstractTableModel(){
	//Store references
	this->analysisInfo = analysisInfo;
	this->livelinessDao = livelinessDao;

	//initialise variables
	clusterDisplayIndex = -1;
	maxHeatColorValue = DEFAULT_MAX_HEAT_COLOR_VALUE;
}


/*! Destructor */
LivelinessFullResultsModel::~LivelinessFullResultsModel(){
}


/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int LivelinessFullResultsModel::columnCount(const QModelIndex&) const{
	return numCols;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
	ready to be displayed in the way requested */
QVariant LivelinessFullResultsModel::data(const QModelIndex & index, int role) const{
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
		//Get pointer to the appropriate Cluster class
		const Cluster* tmpCluster = &clusterList[index.row()];

		if(index.column() == idCol)
			return tmpCluster->getID();
		if(index.column() == timeStepCol)
			return tmpCluster->getTimeStep();
		if(index.column() == livelinessCol)
			return tmpCluster->getLiveliness();
		if(index.column() == neurCol)
			return tmpCluster->getNeuronIDString();
	}

	if (role == Qt::DecorationRole){
		if(index.column() == viewCol ){
			if( clusterDisplayIndex == index.row() )
				return QIcon(Globals::getSpikeStreamRoot() + "/images/visible.xpm");
			return QIcon(Globals::getSpikeStreamRoot() + "/images/hidden.xpm");
		}
	}


	//If we have reached this point ignore request
	return QVariant();
}


/*! Returns the maximum neuron liveliness or the default if there are no results */
double LivelinessFullResultsModel::getMaxHeatColorValue(){
	return  maxHeatColorValue;
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant LivelinessFullResultsModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal){
		if(section == idCol)
			return "ID";
		if(section == timeStepCol)
			return "Time Step";
		if(section == livelinessCol)
			return "Liveliness";
		if(section == neurCol)
			return "NeuronIDs";
		if(section == viewCol)
			return "";
	}

	return QVariant();

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int LivelinessFullResultsModel::rowCount(const QModelIndex&) const{
	return clusterList.size();
}


/*! Sets data in the model */
bool LivelinessFullResultsModel::setData(const QModelIndex& index, const QVariant&, int) {
	if (!index.isValid() || !Globals::analysisLoaded() )
		return false;
	if (index.row() < 0 || index.row() >= rowCount())
		return false;

	//Change visibility of cluster
	if(index.column() == viewCol){
		setVisibleCluster(index.row());

		//Call reset to reload table becaus other rows could have been affected
		reset();
		return true;
	}

	//If we have reached this point no data has been set
	return false;
}


/*! Reloads list of clusters from the database without altering the display settings */
void LivelinessFullResultsModel::reload(){
	//Clear everything if no analysis is loaded
	if(analysisInfo->getID() == 0){
		clearClusters();
		reset();
		return;
	}

	//Get the current list of clusters and the maximum neuron liveliness
	QList<Cluster> newClusterList;
	try{
		newClusterList = livelinessDao->getClusters(analysisInfo->getID());
		maxHeatColorValue = livelinessDao->getMaxNeuronLiveliness(analysisInfo->getID());
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		clearClusters();
		reset();
		return;
	}

	//Get Id of currently displayed cluster
	if(clusterDisplayIndex > 0){
		unsigned int visClusterID = clusterList[clusterDisplayIndex].getID();

		//Determine if this cluster is in the new list
		int newClusterDisplayIndex = -1;
		for(int i=0; i<newClusterList.size(); ++i){
			if(visClusterID == newClusterList[i].getID()){
				newClusterDisplayIndex = i;
				break;
			}
		}

		//If cluster could not be found, hide cluster
		if(newClusterDisplayIndex < 0){
			setVisibility(clusterList[clusterDisplayIndex], false);
			clusterDisplayIndex = -1;
		}
	}

	//Store new list of clusters
	clusterList = newClusterList;

	//Signal to views using this model
	reset();
}


/*! Sets the maximum heat value and adjusts the color index appropriately */
void LivelinessFullResultsModel::setMaxHeatColorValue(double maxHeatColorValue){
	this->maxHeatColorValue = maxHeatColorValue;

	//Re-show the cluster to set the heat map correctly
	if(clusterDisplayIndex >= 0){
		setVisibility(clusterList[clusterDisplayIndex], false);
		setVisibility(clusterList[clusterDisplayIndex], true);
	}
}


/*----------------------------------------------------------*/
/*-------             PRIVATE METHODS                -------*/
/*----------------------------------------------------------*/

/*! Clears all information stored about clusters and resets the highlight map */
void LivelinessFullResultsModel::clearClusters(){
	//Hide visible cluster
	if(clusterDisplayIndex > 0)
		setVisibility(clusterList[clusterDisplayIndex], false);

	//Clean up the data structures
	clusterList.clear();
	clusterDisplayIndex = -1;
	maxHeatColorValue = DEFAULT_MAX_HEAT_COLOR_VALUE;
}


/*! Makes a new cluster visible or hides the present cluster */
void LivelinessFullResultsModel::setVisibleCluster(int index){
	if(index >= clusterList.size())
		throw SpikeStreamException("Cluster display index is out of range.");

	//Hide cluster if it is already visible
	if(index == clusterDisplayIndex){
		setVisibility(clusterList[clusterDisplayIndex], false);
		clusterDisplayIndex = -1;
	}
	//Hide any currently visible cluster
	else if(index < 0 && clusterDisplayIndex > 0){
		setVisibility(clusterList[clusterDisplayIndex], false);
		clusterDisplayIndex = -1;
	}
	//Different column clicked
	else{
		//Hide current column
		if(clusterDisplayIndex >=0)
			setVisibility(clusterList[clusterDisplayIndex], false);
		setVisibility(clusterList[index], true);
		clusterDisplayIndex = index;
	}
}


/*! Sets the visibility of the cluster with the specified id */
void LivelinessFullResultsModel::setVisibility(Cluster& cluster, bool makeVisible){
	//Hide cluster
	if(!makeVisible){
		Globals::getNetworkDisplay()->removeHighlightNeurons(cluster.getNeuronIDs());
	}
	//Show cluster
	else {
		//Build a new neuron colour map
		QHash<unsigned int, RGBColor*> newColorMap;
		QHash<unsigned int, double> neuronLivelinessMap = cluster.getNeuronLivelinessMap();
		for(QHash<unsigned int, double>::iterator mapIter = neuronLivelinessMap.begin(); mapIter != neuronLivelinessMap.end(); ++mapIter){
			if( mapIter.value() < maxHeatColorValue/10 )
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_0);
			else if( mapIter.value() < 2.0 * (maxHeatColorValue/10.0) )
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_1);
			else if( mapIter.value() < 3.0 * (maxHeatColorValue/10.0) )
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_2);
			else if( mapIter.value() < 4.0 * (maxHeatColorValue/10.0) )
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_3);
			else if( mapIter.value() < 5.0 * (maxHeatColorValue/10.0) )
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_4);
			else if( mapIter.value() < 6.0 * (maxHeatColorValue/10.0) )
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_5);
			else if( mapIter.value() < 7.0 * (maxHeatColorValue/10.0) )
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_6);
			else if( mapIter.value() < 8.0 * (maxHeatColorValue/10.0) )
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_7);
			else if( mapIter.value() < 9.0 * (maxHeatColorValue/10.0) )
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_8);
			else if( mapIter.value() < 10.0 * (maxHeatColorValue/10.0) )
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_9);
			else
				newColorMap[mapIter.key()] = new RGBColor(HEAT_COLOR_10);
		}
		Globals::getNetworkDisplay()->addHighlightNeurons(newColorMap);
	}
}


