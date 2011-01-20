#ifndef IZHIKEVICHNETWORKSBUILDER_H
#define IZHIKEVICHNETWORKSBUILDER_H

//SpikeStream includes
#include "Network.h"
#include "SpikeStreamThread.h"
using namespace spikestream;

//Qt includes
#include <QString>

namespace spikestream {

	class IzhikevichNetworkBuilder : public SpikeStreamThread {
		Q_OBJECT

		public:
			IzhikevichNetworkBuilder();
			~IzhikevichNetworkBuilder();
			void run();
			void startAddPolychronizationNetwork(Network* newNetwork);

		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);

		private:
			//===================  VARIABLES  ====================
			/*! The task being carrried out when the thread is running. */
			int currentTask;

			/*! The network being added */
			Network* newNetwork;

			static const int NO_TASK_DEFINED = 0;
			static const int ADD_POLYCHRONIZATION_NETWORK_TASK = 1;

			//===================  METHODS  ======================
			void addPolychronizationNetwork();
			QString getConGrpDescription(NeuronGroup* fromNeuronGroup, NeuronGroup* toNeuronGroup);
			void setExcitatoryParameters(QHash<QString, double>& parameterMap);
			void setInhibitoryParameters(QHash<QString, double>& parameterMap);

	};

}

#endif//IZHIKEVICHNETWORKSBUILDER_H
