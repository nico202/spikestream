#include "SpikeStreamException.h"
#include "StateBasedPhiAnalysisDao.h"
#include "Util.h"
using namespace spikestream;

/*! Standard Constructor */
StateBasedPhiAnalysisDao::StateBasedPhiAnalysisDao(const DBInfo& dbInfo) : AnalysisDao(dbInfo){
}


/*! Empty constructor for unit testing */
StateBasedPhiAnalysisDao::StateBasedPhiAnalysisDao(){
}


/*! Destructor */
StateBasedPhiAnalysisDao::~StateBasedPhiAnalysisDao(){
}


/*! Adds a complex to the database */
void StateBasedPhiAnalysisDao::addComplex(unsigned int analysisID, int timeStep, QList<unsigned int>& neuronIDList, double phi){
    if(neuronIDList.size() < 2)
	throw SpikeStreamException("A complex must contain at least 2 neurons.");

    //Build query
    QString queryStr = "INSERT INTO StateBasedPhiData (AnalysisID, TimeStep, Phi, Neurons) VALUES (";
    queryStr += QString::number(analysisID) + ", " + QString::number(timeStep) + ", " + QString::number(phi) + ", '";
    QString neuronIDStr = "";
    QList<unsigned int>::iterator endNeuronIDList = neuronIDList.end();
    for(QList<unsigned int>::iterator iter = neuronIDList.begin(); iter != endNeuronIDList; ++iter){
	neuronIDStr += QString::number(*iter) + ",";
    }
    neuronIDStr.truncate(neuronIDStr.length() -1);//Trim off trailing comma
    queryStr += neuronIDStr + "')";
    executeQuery(queryStr);
}


/*! Deletes time steps covering the specified range.
    This range is inclusive, so first and last time steps will be deleted */
void StateBasedPhiAnalysisDao::deleteTimeSteps(unsigned int firstTimeStep, unsigned int lastTimeStep){
    executeQuery("DELETE FROM StateBasedPhiData WHERE TimeStep >= " + QString::number(firstTimeStep) + " AND TimeStep <= " + QString::number(lastTimeStep));
}


/*! Returns the number of complexes within a particular range of time steps */
int StateBasedPhiAnalysisDao::getComplexCount(unsigned int analysisID, unsigned int firstTimeStep, unsigned int lastTimeStep){
    QString queryString = "SELECT COUNT(*) FROM StateBasedPhiData WHERE AnalysisID=" + QString::number(analysisID);
    queryString += " AND TimeStep >= " + QString::number(firstTimeStep) + " AND TimeStep <= " + QString::number(lastTimeStep);
    QSqlQuery query = getQuery(queryString);
    executeQuery(query);
    query.next();
    return query.value(0).toUInt();
}


/*! Returns a list of complexes corresponding to the state based phi data table for a particular analysis */
QList<Complex> StateBasedPhiAnalysisDao::getComplexes(unsigned int analysisID){
    //Query database for matching complexes
    QSqlQuery query = getQuery( "SELECT ComplexID, TimeStep, Phi, Neurons FROM StateBasedPhiData WHERE AnalysisID=" + QString::number(analysisID) + " ORDER BY TimeStep");
    executeQuery(query);

    //Add complexes to list
    QList<Complex> complexList;
    while(query.next()){
	QList<unsigned int> neuronIDList = Util::getUIntList(query.value(3).toString());
	Complex tmpComplex(
		Util::getUInt(query.value(0).toString()),//ID
		Util::getUInt(query.value(1).toString()),//Time step
		Util::getDouble(query.value(2).toString()),//Phi
		neuronIDList//Neuron IDs
	);
	complexList.append(tmpComplex);
    }
    return complexList;
}




