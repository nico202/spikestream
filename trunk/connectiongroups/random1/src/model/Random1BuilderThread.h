#ifndef RANDOM1BUILDERTHREAD_H
#define RANDOM1BUILDERTHREAD_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "AbstractConnectionBuilder.h"


namespace spikestream {

	/*! Adds a connection group to the current network, which automatically adds it to the database */
	class Random1BuilderThread : public AbstractConnectionBuilder {
		Q_OBJECT

		public:
			Random1BuilderThread();
			~Random1BuilderThread();

		protected:
			void buildConnectionGroup();
			void checkParameters();

		private:
			//=======================  VARIABLES  ========================
			/*! Size of the target of the connection projection */
			double targetSize;

			/*! Density of the connections */
			double connectionProbability;

			/*! Total number of progress steps */
			int numberOfProgressSteps;

	};
}

#endif//RANDOM1BUILDERTHREAD_H
