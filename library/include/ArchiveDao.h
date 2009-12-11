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

	    void addArchive(ArchiveInfo& archiveInfo);
	    void addArchiveData(unsigned int archiveID, unsigned int timeStep, const QString& firingNeuronString);
	    void deleteArchive(unsigned int archiveID);
	    QList<ArchiveInfo> getArchivesInfo(unsigned int networkID);
	    int getArchiveSize(unsigned int archiveID);
	    QStringList getFiringNeuronIDs(unsigned int archiveID, unsigned int timeStep);
	    unsigned int getMaxTimeStep(unsigned int archiveID);
	    unsigned int getMinTimeStep(unsigned int archiveID);
            bool networkIsLocked(unsigned int networkID);
	};

}

#endif//ARCHIVEDAO_H

