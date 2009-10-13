#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

/*! Constructor */
StateBasedPhiAnalysisDao::StateBasedPhiAnalysisDao(const DBInfo& dbInfo) : AnalysisDao(dbInfo){

}


/*! Destructor */
StateBasedPhiAnalysisDao::~StateBasedPhiAnalysisDao(){
}


QSqlQueryModel* StateBasedPhiAnalysisDao::getStateBasedPhiDataTableModel(unsigned int analysisID){
    QString queryString = "SELECT ClusterID, TimeStep, Phi, Neurons FROM StateBasedPhiData WHERE AnalysisID=" + QString::number(analysisID);
    QSqlQuery query = getQuery(queryString);
    executeQuery(query);
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query);
    return model;
}


