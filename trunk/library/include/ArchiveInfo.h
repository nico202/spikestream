#ifndef ARCHIVEINFO_H
#define ARCHIVEINFO_H

namespace spikestream {

    class ArchiveInfo {
	public:
	    ArchiveInfo();
	    ArchiveInfo(unsigned int id, unsigned int networkID, const QString& startDateTime, const QString& description, int numberOfEntries = 0);
	    ArchiveInfo(const ArchiveInfo& archInfo);
	    ~ArchiveInfo();
	    ArchiveInfo& operator=(const ArchiveInfo& rhs);

	    unsigned int getID() { return id; }
	    unsigned int getNetworkID() { return networkID; }
	    unsigned int getDate() { return startDateTime; }
	    unsigned int getDescription() { return description; }
	    int size() { return numberOfEntries; }

	private:
	    //=========================  VARIABLES  ===========================
	    /*! ID of the archive in the database */
	    unsigned int id;
	    unsigned int networkID;
	    QString startDateTime;
	    QString description;

	    /*! The number of entries in the archive */
	    int numberOfEntries;

    };

}

#endif//ARCHIVEINFO_H


