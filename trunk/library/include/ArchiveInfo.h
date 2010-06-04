#ifndef ARCHIVEINFO_H
#define ARCHIVEINFO_H

//Qt includes
#include <QString>
#include <QDateTime>

namespace spikestream {

    class ArchiveInfo {
	public:
	    ArchiveInfo();
	    ArchiveInfo(unsigned int id, unsigned int networkID, unsigned int unixTimestamp, const QString& description);
	    ArchiveInfo(const ArchiveInfo& archInfo);
	    ~ArchiveInfo();
	    ArchiveInfo& operator=(const ArchiveInfo& rhs);

	    unsigned int getID() { return id; }
	    unsigned int getNetworkID() { return networkID; }
	    QDateTime getDateTime() { return startDateTime; }
	    QString getDescription() { return description; }
		void reset();
	    void setID(unsigned int id) { this->id = id; }
		void setNetworkID(unsigned int netID) { this->networkID = netID; }

	private:
	    //=========================  VARIABLES  ===========================
	    /*! ID of the archive in the database */
	    unsigned int id;
	    unsigned int networkID;
	    QDateTime startDateTime;
	    QString description;

    };

}

#endif//ARCHIVEINFO_H


