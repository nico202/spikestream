//SpikeStream includes
#include "AnalysisDao.h"
#include "GlobalVariables.h"
#include "SpikeStreamDBException.h"
#include "Util.h"
#include "XMLParameterParser.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Standard constructor */
AnalysisDao::AnalysisDao(const DBInfo& dbInfo) : AbstractDao(dbInfo){
}


/*! Empty constructor for unit testing */
AnalysisDao::AnalysisDao(){
}


/*! Destructor */
AnalysisDao::~AnalysisDao(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

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


/*! Deletes the analysis with the specified ID. Does nothing if no analysis with this ID is in the database */
void AnalysisDao::deleteAnalysis(unsigned int analysisID){
    executeQuery("DELETE FROM Analyses WHERE AnalysisID = " + QString::number(analysisID));
}


/*! Deletes all analyses from the SpikeStreamAnalysis database. */
void AnalysisDao::deleteAllAnalyses(){
	executeQuery("DELETE FROM Analyses");
}


/*! Returns a list of information about analyses matching the network and archive ID */
QList<AnalysisInfo> AnalysisDao::getAnalysesInfo(unsigned int networkID, unsigned int archiveID, unsigned int analysisType){
	QString queryStr = "SELECT AnalysisID, StartTime, Description, Parameters, AnalysisTypeID FROM Analyses ";
	queryStr += "WHERE NetworkID=" + QString::number(networkID) + " AND ArchiveID=" + QString::number(archiveID);
	queryStr += " AND AnalysisTypeID=" + QString::number(analysisType);

	QSqlQuery query = getQuery(queryStr);
    executeQuery(query);
    QList<AnalysisInfo> tmpList;
    for(int i=0; i<query.size(); ++i){
		query.next();

		//Extract the parameters
		XMLParameterParser parser;
		QHash<QString, double> paramMap = parser.getParameterMap(query.value(3).toString());

		//Create the analysis info and add it to the list
		tmpList.append(
				AnalysisInfo(
						Util::getUInt(query.value(0).toString()),//AnalysisID
						networkID,
						archiveID,
						QDateTime::fromTime_t(Util::getUInt(query.value(1).toString())),//Date time
						query.value(2).toString(),//Description
						paramMap,//Parameter map
						Util::getUInt(query.value(4).toString())//Analysis type
						)
				);
    }
    return tmpList;
}



/*! Returns true if the network is associated with analyses and is
	therefore uneditable */
bool AnalysisDao::networkHasAnalyses(unsigned networkID){
	QSqlQuery query = getQuery("SELECT COUNT(*) FROM Analyses WHERE NetworkID=" + QString::number(networkID));
	executeQuery(query);
	query.next();
	if(query.value(0).toUInt() == 0)
		return false;
	return true;
}



/*! Updates the analysis description */
void AnalysisDao::updateDescription(unsigned int analysisID, const QString& newDescription){
    executeQuery("UPDATE Analyses SET Description='" + newDescription + "' WHERE AnalysisID=" + QString::number(analysisID));
}


