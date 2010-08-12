#ifndef RANDOM1BUILDERTHREAD_H
#define RANDOM1BUILDERTHREAD_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"

//Qt includes
#include <QThread>


namespace spikestream {

	/*! Adds a connection group to the current network, which automatically adds it to the database */
	class Random1BuilderThread : public QThread {
		Q_OBJECT

		public:
			Random1BuilderThread();
			~Random1BuilderThread();
			void cancel();
			QString getErrorMessage(){ return errorMessage; }
			bool isError() { return error; }
			void prepareAddConnectionGroup(const ConnectionGroupInfo& conGrpInfo);
			void run();
			void stop();


		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);


		private:
			//=======================  VARIABLES  ========================
			/*! Holds information about the connection group that is being added */
			ConnectionGroupInfo connectionGroupInfo;

			/*! Connection group being added */
			ConnectionGroup* newConnectionGroup;

			/*! Network Dao used by Network when in thread */
			NetworkDao* threadNetworkDao;

			/*! Archive Dao used by Network when in thread */
			ArchiveDao* threadArchiveDao;

			/*! Set to true when network has finished adding the connection groups */
			bool networkFinished;

			/*! Used to cancel the operation */
			bool stopThread;

			/*! Set to true when an error has occurred */
			bool error;

			/*! Message associated with an error */
			QString errorMessage;

			/*! Size of the target of the connection projection */
			double targetSize;

			/*! Density of the connections */
			double connectionProbability;

			/*! Total number of progress steps */
			int numberOfProgressSteps;


			//=======================  METHODS  ==========================
			void addConnectionGroup();
			void buildConnectionGroup();
			void checkParameters();
			void clearError();
			double getParameter(const QString& paramName);
			double getRandomDouble(double min, double max);
			unsigned int getRandomUInt(unsigned int min, unsigned int max);
			unsigned int getToNeuronID(NeuronGroup* neurGrp);
			void setError(const QString& errorMessage);

	};
}

#endif//RANDOM1BUILDERTHREAD_H
