#include "LivelinessDao.h"
#include "SpikeStreamException.h"
#include "SpikeStreamDBException.h"
#include "Util.h"
using namespace spikestream;

/*! Standard Constructor */
LivelinessDao::LivelinessDao(const DBInfo& dbInfo) : AnalysisDao(dbInfo){
}


/*! Empty constructor for unit testing */
LivelinessDao::LivelinessDao(){
}


/*! Destructor */
LivelinessDao::~LivelinessDao(){
}


/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Adds a new cluster to the database */
void LivelinessDao::addCluster(unsigned int analysisID, int timeStep, QList<unsigned int>& neuronIDList, double liveliness){
	//Build query
	QString queryStr = "INSERT INTO ClusterLiveliness (AnalysisID, TimeStep, Liveliness, Neurons) VALUES (";
	queryStr += QString::number(analysisID) + ", " + QString::number(timeStep) + ", " + QString::number(liveliness) + ", '";
	QString neuronIDStr = "";
	QList<unsigned int>::iterator endNeuronIDList = neuronIDList.end();
	for(QList<unsigned int>::iterator iter = neuronIDList.begin(); iter != endNeuronIDList; ++iter){
		neuronIDStr += QString::number(*iter) + ",";
	}
	neuronIDStr.truncate(neuronIDStr.length() -1);//Trim off trailing comma
	queryStr += neuronIDStr + "')";
	executeQuery(queryStr);
}


/*! Deletes cluster and neuron data for the supplied range of time steps.
	The range is inclusive, so start and end time steps will both be deleted. */
void LivelinessDao::deleteTimeSteps(unsigned int analysisID, unsigned int firstTimeStep, unsigned int lastTimeStep){
	executeQuery("DELETE FROM ClusterLiveliness WHERE AnalysisID=" + QString::number(analysisID) + " AND TimeStep >= " + QString::number(firstTimeStep) + " AND TimeStep <= " + QString::number(lastTimeStep));
	executeQuery("DELETE FROM NeuronLiveliness WHERE AnalysisID=" + QString::number(analysisID) + " AND TimeStep >= " + QString::number(firstTimeStep) + " AND TimeStep <= " + QString::number(lastTimeStep));
}


/*! Checks to see if there is already analysis data for this range of time steps.
	Range includes start and end time step. */
bool LivelinessDao::containsAnalysisData(unsigned int analysisID, unsigned int firstTimeStep, unsigned int lastTimeStep){
	//Check the cluster liveliness table
	QString queryString = "SELECT COUNT(*) FROM ClusterLiveliness WHERE AnalysisID=" + QString::number(analysisID);
	queryString += " AND TimeStep >= " + QString::number(firstTimeStep) + " AND TimeStep <= " + QString::number(lastTimeStep);
	QSqlQuery query = getQuery(queryString);
	executeQuery(query);
	query.next();

	//Return true if we have found analysis data
	if(query.value(0).toUInt() != 0)
		return true;

	//Check the neuron liveliness table
	queryString = "SELECT COUNT(*) FROM NeuronLiveliness WHERE AnalysisID=" + QString::number(analysisID);
	queryString += " AND TimeStep >= " + QString::number(firstTimeStep) + " AND TimeStep <= " + QString::number(lastTimeStep);
	query = getQuery(queryString);
	executeQuery(query);
	query.next();

	//Return true if we have found analysis data, or false if not
	if(query.value(0).toUInt() != 0)
		return true;
	return false;
}


/*! Returns all of the clusters for the analysis */
QList<Cluster> LivelinessDao::getClusters(unsigned int analysisID){
	//Query database for matching clusters
	QSqlQuery query = getQuery( "SELECT ClusterID, TimeStep, Liveliness, Neurons FROM ClusterLiveliness WHERE AnalysisID=" + QString::number(analysisID) + " ORDER BY TimeStep");
	executeQuery(query);

	//Add clusters to list
	QList<Cluster> clusterList;
	while(query.next()){
		//Time step of this cluster
		unsigned int tmpTimeStep = Util::getUInt(query.value(1).toString());

		//Get list of neuron ids
		QList<unsigned int> neuronIDList = Util::getUIntList(query.value(3).toString());

		//Get the liveliness associated with each neuron
		QHash<unsigned int, double> tmpNeurLivelinessMap;
		foreach(unsigned int neurID, neuronIDList){
			tmpNeurLivelinessMap[neurID] = getNeuronLiveliness(analysisID, tmpTimeStep, neurID);
		}

		//Create the cluster and add to list
		Cluster tmpCluster(
				Util::getUInt(query.value(0).toString()),//ID
				tmpTimeStep,//Time step
				Util::getDouble(query.value(2).toString()),//Liveliness
				tmpNeurLivelinessMap//Neuron IDs associated with an individual color
		);
		clusterList.append(tmpCluster);
	}
	return clusterList;
}


/*! Returns the liveliness associated with a particular neuron and time step. */
double LivelinessDao::getNeuronLiveliness(unsigned int analysisID, unsigned int timeStep, unsigned int neuronID){
	QString queryStr = "SELECT Liveliness FROM NeuronLiveliness WHERE AnalysisID=" + QString::number(analysisID) + " AND ";
	queryStr += "TimeStep=" + QString::number(timeStep) + " AND NeuronID=" + QString::number(neuronID);
	QSqlQuery query = getQuery(queryStr);
	executeQuery(query);
	if(query.size() != 1)
		throw SpikeStreamDBException("Cannot find neuron liveliness. Query String: " + queryStr);
	query.next();
	return Util::getDouble(query.value(0).toString());
}


/*! Returns the maximum neuron liveliness in the analysis */
double LivelinessDao::getMaxNeuronLiveliness(unsigned int analysisID){
	QSqlQuery query = getQuery("SELECT MAX(Liveliness) FROM NeuronLiveliness WHERE AnalysisID=" + QString::number(analysisID));
	executeQuery(query);
	query.next();
	return Util::getDouble(query.value(0).toString());
}


/*! Sets the neuron's liveliness for a given time step and analysis.
	If the neuron's liveliness has already been set, it is updated. */
void LivelinessDao::setNeuronLiveliness(unsigned int analysisID, int timeStep, unsigned int neuronID, double liveliness){
	QString queryStr = "INSERT INTO NeuronLiveliness (NeuronID, AnalysisID, TimeStep, Liveliness) VALUES (";
	queryStr += QString::number(neuronID) + "," + QString::number(analysisID) + "," + QString::number(timeStep) + "," + QString::number(liveliness) + ") ";
	queryStr += "ON DUPLICATE KEY UPDATE Liveliness=" + QString::number(liveliness);
	executeQuery(queryStr);
}



