#ifndef RANDOM1BUILDERTHREAD_H
#define TOPOGRAPHIC1BUILDERTHREAD_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "AbstractConnectionBuilder.h"


namespace spikestream {

	/*! Adds a connection group with topographic connections to the current network,
		which automatically adds it to the database if this is appropriate. */
	class Topographic1BuilderThread : public AbstractConnectionBuilder {
		Q_OBJECT

		public:
			Topographic1BuilderThread();
			~Topographic1BuilderThread();

		protected:
			void buildConnectionGroup();
			void checkParameters();

		private:
			//=======================  VARIABLES  ========================
			/*! Total number of progress steps */
			int numberOfProgressSteps;

	};
}

#endif//TOPOGRAPHIC1BUILDERTHREAD_H
