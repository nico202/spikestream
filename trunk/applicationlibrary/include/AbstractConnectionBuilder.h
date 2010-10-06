#ifndef ABSTRACTCONNECTIONBUILDER_H
#define ABSTRACTCONNECTIONBUILDER_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "ConnectionGroupInfo.h"
#include "NetworkDao.h"
#include "SpikeStreamThread.h"

//Qt includes
#include <QHash>

namespace spikestream {

	/*! Abstract class with functionality common to all classes that build
		connections in a separate thread and potentially add them to the database. */
	class AbstractConnectionBuilder : public SpikeStreamThread {
		Q_OBJECT

		public:
			AbstractConnectionBuilder();
			virtual ~AbstractConnectionBuilder();
			virtual void startBuildConnectionGroup(const ConnectionGroupInfo& conGrpInfo);
			virtual void run();


		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);


		protected:
			//=================  VARIABLES  ======================
			/*! Holds information about the connection group that is being added */
			ConnectionGroupInfo connectionGroupInfo;

			/*! Connection group being added */
			ConnectionGroup* newConnectionGroup;

			/*! Network Dao used by Network when in thread */
			NetworkDao* threadNetworkDao;


			//===================  METHODS  ======================
			virtual void buildConnectionGroup() = 0;
			virtual void checkParameters() = 0;
			virtual double getParameter(const QString& paramName);

	};

}

#endif//ABSTRACTCONNECTIONBUILDER_H
