#ifndef ARCHIVEINFO_H
#define ARCHIVEINFO_H

//Qt includes
#include <QString>
#include <QDateTime>

namespace spikestream {

    class ArchiveInfo {
	public:
	    ArchiveInfo();
	    ArchiveInfo(unsigned int id, unsigned int networkID, unsigned int unixTimestamp, const QString& description, int numberOfEntries = 0);
	    ArchiveInfo(const ArchiveInfo& archInfo);
	    ~ArchiveInfo();
	    ArchiveInfo& operator=(const ArchiveInfo& rhs);

	    unsigned int getID() { return id; }
	    unsigned int getNetworkID() { return networkID; }
	    QDateTime getDateTime() { return startDateTime; }
	    QString getDescription() { return description; }
	    int size() { return numberOfEntries; }

	private:
	    //=========================  VARIABLES  ===========================
	    /*! ID of the archive in the database */
	    unsigned int id;
	    unsigned int networkID;
	    QDateTime startDateTime;
	    QString description;

	    /*! The number of entries in the archive */
	    int numberOfEntries;

    };

}

#endif//ARCHIVEINFO_H


