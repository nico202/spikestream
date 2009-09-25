#ifndef ARCHIVEDAO_H
#define ARCHIVEDAO_H

#include "ArchiveInfo.h"
#include "AbstractDao.h"
#include "DBInfo.h"
using namespace spikestream;

namespace spikestream {

    class ArchiveDao : public AbstractDao {
	public:
	    ArchiveDao(const DBInfo& dbInfo);
	    virtual ~ArchiveDao();

	    void deleteArchive(unsigned int archiveID);
	    QList<ArchiveInfo> getArchivesInfo(unsigned int networkID);
	    int getArchiveSize(unsigned int archiveID);
    };

}

#endif//ARCHIVEDAO_H

