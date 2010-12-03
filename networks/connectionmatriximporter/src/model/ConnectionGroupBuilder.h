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
			double delay;
			double threshold;
			ConnectionGroup* connectionGroup;
	};


	/*! Adds connections to the supplied network using the loaded data about connection weights and delays. */
	class ConnectionGroupBuilder : public QObject {
		Q_OBJECT

		public:
			ConnectionGroupBuilder(QList<NeuronGroup*> excitNeurGrpList, QList<NeuronGroup*> inhibNeurGrpList);
			~ConnectionGroupBuilder();
			void addConnectionGroups(Network* network, bool* stopThread, const QString& weightsFilePath, const QString& delaysFilePath, QHash<QString, double>& parameterMap, urng_t& ranNumGen);


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

			/*! List linking neuron group indexes with the connections that connect TO the node. */
			QList< QList<ConnectionInfo> > toNeurGrpConList;

			/*! Map of parameters, stored in connection groups. */
			QHash<QString, double> parameterMap;

			/*! The synapse type of all the connections */
			SynapseType synapseType;

			/*! Threshold above which rewiring takes place */
			double rewireProbability;

			/*! Minimum delay for within node connections. */
			double minDelayRange;

			/*! Maximum delay for within node connections. */
			double maxDelayRange;

			/*! Speed at which a spike propagates along an axon in metres per second. */
			double spikePropagationSpeed;

			/*! Minimum weight of an excitatory connection */
			float minExcitatoryWeight;

			/*! Maximum weight of an excitatory connection. */
			float maxExcitatoryWeight;

			/*! Minimum weight of an inhibitory connection. */
			float minInhibitoryWeight;

			/*! Maximum weight of an inhibitory connection. */
			float maxInhibitoryWeight;

			/*! Rewires to connections */
			bool rewireToConnections;

			/*! Default synapse parameters. The key is the synapse type ID. The value is the map of default parameters. */
			QHash<unsigned, QHash<QString, double> > defaultParameterMaps;


			//======================  METHODS  ===========================
			void addConnections(Network* network, int nodeIndex, urng_t& ranNumGen);
			void addInhibitoryConnections(Network* network, NeuronGroup* excitNeurGrp, NeuronGroup* inhibNeurGrp, urng_t& ranNumGen);
			void calculateToConnectionList();
			void calculateThresholds();
			QString getConGrpDescription(NeuronGroup* fromNeuronGroup, NeuronGroup* toNeuronGroup);
			float getInterDelay(double randomNum, double connectionDelay);
			float getIntraDelay(double randomNum);
			unsigned getRandomExcitatoryNeuronID(int index);
			float getExcitatoryWeight(double randomNum);
			float getInhibitoryWeight(double randomNum);
			void loadDefaultParameters();
			void loadDelays(const QString& delaysFilePath);
			void loadWeights(const QString& weightsFilePath);
			void normalizeWeights();
			void printConnections(bool printToFile = true);
			void storeParameters();

	};

}

#endif//CONNECTIONGROUPBUILDER_H
