#include "ArchiveDao.h"
#include "GlobalVariables.h"
#include "SpikeStreamDBException.h"
#include "Util.h"
using namespace spikestream;

/*! Constructor */
ArchiveDao::ArchiveDao(const DBInfo& dbInfo) : AbstractDao(dbInfo){
}


/*! Destructor */
ArchiveDao::~ArchiveDao(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds the archive with the specified ID */
void ArchiveDao::addArchive(ArchiveInfo& archInfo){
	//Get date at which archive is being created.
	archInfo.setDateTime(QDateTime::currentDateTime());

	//Create the archive
    QSqlQuery query = getQuery("INSERT INTO Archives (StartTime, NetworkID, Description) VALUES (" + QString::number(archInfo.getDateTime().toTime_t()) + ", " + QString::number(archInfo.getNetworkID()) + ", '" + archInfo.getDescription() + "')");
    executeQuery(query);

	//Check id is correct and add to archive info if it is
    int lastInsertID = query.lastInsertId().toInt();
    if(lastInsertID >= START_ARCHIVE_ID)
		archInfo.setID(lastInsertID);
    else
		throw SpikeStreamDBException("Insert ID for Archives is invalid: " + QString::number(lastInsertID));
}


/*! Adds data to the archive with the specified ID.*/
void ArchiveDao::addArchiveData(unsigned int archiveID, unsigned int timeStep, const QString& firingNeuronString){
    executeQuery("INSERT INTO ArchiveData(ArchiveID, TimeStep, FiringNeurons) VALUES (" + QString::number(archiveID) + ", " + QString::number(timeStep) + ", '"  + firingNeuronString + "')");
}


/*! Adds data to the archive with the specified ID.*/
void ArchiveDao::addArchiveData(unsigned int archiveID, unsigned int timeStep, const QList<unsigned>& firingNeuronList){

//	QByteArray ba;
//	QFile f(fileName);
//	if(f.open(QIODevice::ReadOnly))
//	{
//	ba = f.readAll();
//	f.close();
//	}
//
//	// Writing the image into table
//	QSqlDatabase::database().transaction();
//	QSqlQuery query;
//	query.prepare( "INSERT INTO picture ( IMAGE ) VALUES (:IMAGE)" );
//	query.bindValue(":IMAGE", ba);

	//FIXME: SORT THIS OUT WHEN I CONVERT TO BLOB
	//Build firing neuron string
	QString tmpStr;
	QList<unsigned>::const_iterator endList = firingNeuronList.end();
	for(QList<unsigned>::const_iterator iter = firingNeuronList.begin(); iter != endList; ++iter)
		tmpStr += QString::number(*iter) + ",";
	tmpStr.truncate(tmpStr.size() - 1);
	addArchiveData(archiveID, timeStep, tmpStr);
}


/*! Deletes the archive with the specified ID */
void ArchiveDao::deleteArchive(unsigned int archiveID){
    executeQuery("DELETE FROM Archives WHERE ArchiveID = " + QString::number(archiveID));
}


/*! Deletes all archives in the database. */
void ArchiveDao::deleteAllArchives(){
	executeQuery("DELETE FROM Archives");
}


/*! Returns a list of the archives in the database that are associated with the specified network. */
QList<ArchiveInfo> ArchiveDao::getArchivesInfo(unsigned int networkID){
    QSqlQuery query = getQuery("SELECT ArchiveID, StartTime, Description FROM Archives WHERE NetworkID=" + QString::number(networkID) + " ORDER BY StartTime");
    executeQuery(query);
    QList<ArchiveInfo> tmpList;
    for(int i=0; i<query.size(); ++i){
		query.next();
		unsigned int archiveID = Util::getUInt(query.value(0).toString());
		tmpList.append(
				ArchiveInfo(
						archiveID,
						networkID,
						Util::getUInt(query.value(1).toString()),//Start time as a unix timestamp
						query.value(2).toString()//Description
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
    unsigned int archiveSize = Util::getInt(query.value(0).toString());
    return archiveSize;
}


/*! Returns the maximum time step in the archive */
unsigned int ArchiveDao::getMaxTimeStep(unsigned int archiveID){
    QSqlQuery query = getQuery("SELECT MAX(TimeStep) FROM ArchiveData WHERE ArchiveID=" + QString::number(archiveID));
    executeQuery(query);
    query.next();
    return Util::getUInt(query.value(0).toString());
}


/*! Returns the minimum time step in the archive */
unsigned int ArchiveDao::getMinTimeStep(unsigned int archiveID){
    QSqlQuery query = getQuery("SELECT MIN(TimeStep) FROM ArchiveData WHERE ArchiveID=" + QString::number(archiveID));
    executeQuery(query);
    query.next();
    return Util::getUInt(query.value(0).toString());
}


/*! Returns a list of firing neuron IDs */
QList<unsigned> ArchiveDao::getFiringNeuronIDs(unsigned int archiveID, unsigned int timeStep){
    QSqlQuery query = getQuery("SELECT FiringNeurons FROM ArchiveData WHERE TimeStep=" + QString::number(timeStep) + " AND ArchiveID=" + QString::number(archiveID));
    executeQuery(query);
	query.next();

	//Build list of firing neuron ids
	QList<unsigned> newList;
	QStringList strList = query.value(0).toString().split(",", QString::SkipEmptyParts);
	QStringListIterator iter(strList);
	while (iter.hasNext()){
		newList.append(Util::getUInt(iter.next()));
	}

	//Return the list we have built
	return newList;
}


/*! Returns true if the network is associated with archives and is
    therefore uneditable */
bool ArchiveDao::networkHasArchives(unsigned int networkID){
    QSqlQuery query = getQuery("SELECT COUNT(*) FROM Archives WHERE NetworkID=" + QString::number(networkID));
    executeQuery(query);
    query.next();
    if(query.value(0).toUInt() == 0)
		return false;
    return true;
}


/*! Sets the archive's properties, currently just the description. */
void ArchiveDao::setArchiveProperties(unsigned archiveID, const QString &description){
	executeQuery("UPDATE Archives SET Description='" + description + "' WHERE ArchiveID=" + QString::number(archiveID));
}



