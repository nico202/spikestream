#include "SpikeStreamException.h"
#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

/*! Constructor */
StateBasedPhiAnalysisDao::StateBasedPhiAnalysisDao(const DBInfo& dbInfo) : AnalysisDao(dbInfo){

}


/*! Destructor */
StateBasedPhiAnalysisDao::~StateBasedPhiAnalysisDao(){
}


/*! Adds a complex to the database */
void StateBasedPhiAnalysisDao::addComplex(unsigned int analysisID, int timeStep, double phi, QList<unsigned int>& neuronIDList){
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


/*! Returns the number of complexes within a particular range of time steps */
int StateBasedPhiAnalysisDao::getComplexCount(unsigned int analysisID, unsigned int firstTimeStep, unsigned int lastTimeStep){
    QString queryString = "SELECT COUNT(*) FROM StateBasedPhiData WHERE AnalysisID=" + QString::number(analysisID);
    queryString += " AND TimeStep >= " + QString::number(firstTimeStep) + " AND TimeStep <= " + QString::number(lastTimeStep);
    QSqlQuery query = getQuery(queryString);
    executeQuery(query);
    query.next();
    return query.value(0).toUInt();
}


/*! Returns a model corresponding to the state based phi data table for a particular analysis */
QSqlQueryModel* StateBasedPhiAnalysisDao::getStateBasedPhiDataTableModel(unsigned int analysisID){
    QString queryString = "SELECT ComplexID, TimeStep, Phi, Neurons FROM StateBasedPhiData WHERE AnalysisID=" + QString::number(analysisID);
    QSqlQuery query = getQuery(queryString);
    executeQuery(query);
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query);
    return model;
}




