#ifndef ARCHIVEDAO_H
#define ARCHIVEDAO_H

//SpikeStream includes
#include "ArchiveInfo.h"
#include "AbstractDao.h"
#include "DBInfo.h"
using namespace spikestream;

namespace spikestream {

	/*! Data access object for the SpikeStreamArchive database */
    class ArchiveDao : public AbstractDao {
		public:
			ArchiveDao(const DBInfo& dbInfo);
			virtual ~ArchiveDao();

			void addArchive(ArchiveInfo& archiveInfo);
			void addArchiveData(unsigned int archiveID, unsigned int timeStep, const QString& firingNeuronString);
			void addArchiveData(unsigned int archiveID, unsigned int timeStep, const QList<unsigned>& firingNeuronList);//UNTESTED
			void deleteArchive(unsigned int archiveID);
			QList<ArchiveInfo> getArchivesInfo(unsigned int networkID);
			int getArchiveSize(unsigned int archiveID);
			QList<unsigned> getFiringNeuronIDs(unsigned int archiveID, unsigned int timeStep);
			unsigned int getMaxTimeStep(unsigned int archiveID);
			unsigned int getMinTimeStep(unsigned int archiveID);
			bool networkIsLocked(unsigned int networkID);
			void setArchiveDescription(unsigned archiveID, const QString& description);//UNTESTED
		};

}

#endif//ARCHIVEDAO_H

