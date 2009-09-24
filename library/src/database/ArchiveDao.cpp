#include "ArchiveDao.h"
#include "Util.h"
using namespace spikestream;

/*! Constructor */
ArchiveDao::ArchiveDao(const DBInfo& dbInfo) : AbstractDao(dbInfo){
}


/*! Destructor */
ArchiveDao::~ArchiveDao(){
}


/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Deletes the archive with the specified ID */
void ArchiveDao::deleteArchive(unsigned int archiveID){
    executeQuery("DELETE FROM Archives WHERE ArchiveID = " + QString::number(archiveID));
}


/*! Returns a list of the archives in the database that are associated with the specified network. */
QList<ArchiveDaoInfo> ArchiveDao::getArchivesInfo(unsigned int networkID){
    QSqlQuery query = getQuery("SELECT ArchiveID, FROM_UNIXTIME(StartTime), Description FROM Archives WHERE NetworkID=" + QString::number(networkID) + " ORDER BY StartTime");
    executeQuery(query);
    QList<ArchiveInfo> tmpList;
    for(int i=0; i<query.size(); ++i){
	query.next();
	unsigned int archiveID = Util::getUInt(query.value(0).toString());
	int archiveSize = getArchiveSize(archiveID);
	tmpList.append(
		ArchiveInfo(
			archiveID,
			networkID,
			query.value(1).toString(),//Start time in date format
			query.value(2).toString(),//Description
			archiveSize
		)
	);
    }
    return tmpList;
}


/*! Returns the number of data rows in the specified archive */
int ArchiveDao::getArchiveSize(unsigned int archiveID){
    QSqlQuery query = getQuery("SELECT COUNT(*) FROM ArchiveData WHERE ArchiveID=" + QString::number(archiveID));
    executeQuery(query);
    query.next();
    return Util::getInt(query.value(0).toString());
}


