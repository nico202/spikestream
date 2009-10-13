//SpikeStream includes
#include "AnalysisDao.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
AnalysisDao::AnalysisDao(const DBInfo& dbInfo) : AbstractDao(dbInfo){
}


/*! Destructor */
AnalysisDao::~AnalysisDao(){
}


QSqlQueryModel* AnalysisDao::getAnalysesTableModel(unsigned int networkID, unsigned int archiveID){
    QSqlQueryModel *model = new QSqlQueryModel;
    QString queryString = "SELECT AnalysisID, FROM_UNIXTIME(StartTime), Description, Parameters, AnalysisTypeID FROM Analyses WHERE NetworkID=" + QString::number(networkID) + " AND ArchiveID=" + QString::number(archiveID);
    QSqlQuery query = getQuery(queryString);
    executeQuery(query);
    model->setQuery(query);
    return model;
}


