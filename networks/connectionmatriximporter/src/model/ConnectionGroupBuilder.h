#ifndef CONNECTIONGROUPBUILDER_H
#define CONNECTIONGROUPBUILDER_H

//SpikeStream includes
#include "Network.h"
#include "NeuronGroup.h"
#include "SynapseType.h"

//Qt includes
#include <QHash>
#include <QObject>

//Other includes
#include "boost/random.hpp"

/*! The random number generator type */
typedef boost::mt19937 rng_t;

/*! The variate generator type, which includes the random number generator type. */
typedef boost::variate_generator<rng_t&, boost::uniform_real<double> > urng_t;

namespace spikestream {

	/*! Simple class to hold information about the loaded connections */
	class ConnectionInfo {
		public:
			ConnectionInfo(int fromIndex, int toIndex, float delay, float weight) {
				this->fromIndex = fromIndex;
				this->toIndex = toIndex;
				this->weight = weight;
				this->delay = delay;
				this->threshold = 0;
				this->connectionGroup = NULL;
			}
			ConnectionInfo(const ConnectionInfo& conInfo){
				this->fromIndex = conInfo.fromIndex;
				this->toIndex = conInfo.toIndex;
				this->weight = conInfo.weight;
				this->delay = conInfo.delay;
				this->threshold = conInfo.threshold;
				this->connectionGroup = conInfo.connectionGroup;
			}
			ConnectionInfo& operator=(const ConnectionInfo& rhs){
				if(this == &rhs)
					return *this;

				this->fromIndex = rhs.fromIndex;
				this->toIndex = rhs.toIndex;
				this->weight = rhs.weight;
				this->delay = rhs.delay;
				this->threshold = rhs.threshold;
				this->connectionGroup = rhs.connectionGroup;

				return *this;
			}

			int fromIndex;
			int toIndex;
			float weight;
			float delay;
			double threshold;
			ConnectionGroup* connectionGroup;
	};


	/*! Adds connections to the supplied network using the loaded data about connection weights and delays. */
	class ConnectionGroupBuilder : public QObject {
		Q_OBJECT

		public:
			ConnectionGroupBuilder(QList<NeuronGroup*> excitNeurGrpList, QList<NeuronGroup*> inhibNeurGrpList);
			~ConnectionGroupBuilder();
			void addConnectionGroups(Network* network, bool* stopThread, const QString& weightsFilePath, const QString& delaysFilePath, QHash<QString, double>& parameterMap);


		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);


		private:
			//======================  VARIABLES  =========================
			/*! Pointer to stop thread in invoking class to enable cancellation. */
			bool* stopThread;

			/*! List of nodes names in the order in which they appear in the connectivity matrix. */
			QList<QString> nodeNamesList;

			/*! Number of nodes in the connection matrix */
			int numberOfNodes;

			/*! List of excitatory neuron groups in order of indexes. */
			QList<NeuronGroup*> excitNeurGrpList;

			/*! List of inhibitory neuron groups in order of indexes. */
			QList<NeuronGroup*> inhibNeurGrpList;

			/*! List linking neuron group indexes with lists of connections. Connection struct holds to neuron group index and weight and delay. */
			QList< QList<ConnectionInfo> > neurGrpConList;

			/*! Map of parameters, stored in connection groups. */
			QHash<QString, double> parameterMap;

			/*! The synapse type of all the connections */
			SynapseType synapseType;

			/*! Threshold above which rewiring takes place */
			double rewireThreshold;

			/*! Minimum delay for within node connections. */
			float minIntraDelay;

			/*! Maximum delay for within node connections. */
			float maxIntraDelay;

			/*! Minimum weight of an excitatory connection */
			float minExcitatoryWeight;

			/*! Maximum weight of an excitatory connection. */
			float maxExcitatoryWeight;

			/*! Minimum weight of an inhibitory connection. */
			float minInhibitoryWeight;

			/*! Maximum weight of an inhibitory connection. */
			float maxInhibitoryWeight;


			//======================  METHODS  ===========================
			void addConnections(Network* network, int nodeIndex, urng_t& ranNumGen);
			void addInhibitoryConnections(Network* network, NeuronGroup* excitNeurGrp, NeuronGroup* inhibNeurGrp);
			void calculateThresholds();
			QString getConGrpDescription(NeuronGroup* fromNeuronGroup, NeuronGroup* toNeuronGroup);
			unsigned getRandomExcitatoryNeuronID(int index);
			void loadDelays(const QString& delaysFilePath);
			void loadWeights(const QString& weightsFilePath);
			void normalizeWeights();
			void printConnections(bool printToFile = true);
			void storeParameters();

	};

}

#endif//CONNECTIONGROUPBUILDER_H
