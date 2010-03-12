#ifndef NETWORKSBUILDER_H
#define NETWORKSBUILDER_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "NetworkDaoThread.h"
using namespace spikestream;


namespace spikestream {

	class NetworksBuilder {
		public:
			NetworksBuilder();
			virtual ~NetworksBuilder();
			bool isError() { return error; }
			QString getErrorMessage() { return errorMessage; }

		protected:
			//====================  VARIABLES  ==========================
			/*! NetworkDao that this class uses - don't use Globals in case we are a separate thread. */
			NetworkDao* networkDao;

			/*! Archive dao that this class uses - don't use Globals in case we are a separate thread. */
			ArchiveDao* archiveDao;


			//=====================  METHODS  ===========================
			virtual void addTraining(unsigned int neuronID, QString trainingStr, bool output);
			virtual void addConnectionGroup(unsigned int networkID, ConnectionGroup& connGrp);
			virtual void runThread(NetworkDaoThread& thread);
			void clearError();
			void setError(const QString& errMsg);

		private:
			//=====================  VARIABLES  =========================
			/*! Records if there has been an error when running as a thread */
			bool error;

			/*! Message associated with an error */
			QString errorMessage;
		};

}

#endif//NETWORKSBUILDER_H

