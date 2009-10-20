//SpikeStream includes
#include "AnalysisDao.h"
#include "GlobalVariables.h"
#include "SpikeStreamDBException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
AnalysisDao::AnalysisDao(const DBInfo& dbInfo) : AbstractDao(dbInfo){
}


/*! Destructor */
AnalysisDao::~AnalysisDao(){
}


/*! Adds the specified analysis to the database */
void AnalysisDao::addAnalysis(AnalysisInfo& analysisInfo){
    QString tmpQStr = "INSERT INTO Analyses (NetworkID, ArchiveID, StartTime, Description, Parameters, AnalysisTypeID) VALUES (";
    tmpQStr += QString::number(analysisInfo.getNetworkID()) + ", ";
    tmpQStr += QString::number(analysisInfo.getArchiveID()) + ", ";
    tmpQStr += QString::number(analysisInfo.getStartDateTime().toTime_t()) + ", ";
    tmpQStr += "'" + analysisInfo.getDescription() + "', ";
    tmpQStr += "'" + analysisInfo.getParameterXML() + "', ";
    tmpQStr += QString::number(analysisInfo.getAnalyisType()) + ")";
    QSqlQuery query = getQuery(tmpQStr);
    executeQuery(query);

    //Check id is correct and add to network info if it is
    int lastInsertID = query.lastInsertId().toInt();
    if(lastInsertID >= START_ANALYSIS_ID)
	analysisInfo.setID(lastInsertID);
    else
	throw SpikeStreamDBException("Insert ID for Analyses is invalid: " + QString::number(lastInsertID));
}


/*! Returns a table model describing all of the analyses for the specified network and archive */
QSqlQueryModel* AnalysisDao::getAnalysesTableModel(unsigned int networkID, unsigned int archiveID){
    QSqlQueryModel *model = new QSqlQueryModel;
    QString queryString = "SELECT AnalysisID, FROM_UNIXTIME(StartTime), Description, Parameters, AnalysisTypeID FROM Analyses WHERE NetworkID=" + QString::number(networkID) + " AND ArchiveID=" + QString::number(archiveID);
    QSqlQuery query = getQuery(queryString);
    executeQuery(query);
    model->setQuery(query);
    return model;
}


