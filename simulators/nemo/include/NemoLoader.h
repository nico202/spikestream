#ifndef NEMOLOADER_H
#define NEMOLOADER_H

//SpikeStream includes
#include "Network.h"
using namespace spikestream;

//Nemo includes
#include "nemo.h"

//Qt includes
#include <QObject>

//Other includes
#include "boost/random.hpp"


/*! The random number generator type */
typedef boost::mt19937 rng_t;

/*! The variate generator type, which includes the random number generator type. */
typedef boost::variate_generator<rng_t&, boost::uniform_real<double> > urng_t;


namespace spikestream {

	/*! Loads the network into the graphics hardware ready to run with Nemo */
	class NemoLoader : public QObject {
		Q_OBJECT

		public:
			NemoLoader();
			~NemoLoader();
			nemo_network_t buildNemoNetwork(Network* network, QHash<unsigned, synapse_id*>& volatileConGrpMap, const bool* stop);

		signals:
			void progress(int stepsCompleted, int totalSteps);

		private:
			//======================  VARIABLES  =======================
			/*! File where log is written if required. */
			QFile* logFile;

			/*! Text stream connected to log file */
			QTextStream* logTextStream;

			//======================  METHODS  =======================
			void addExcitatoryNeuronGroup(NeuronGroup* neuronGroup, nemo_network_t nemoNetwork, urng_t& ranNumGen);
			void addInhibitoryNeuronGroup(NeuronGroup* neuronGroup, nemo_network_t nemoNetwork, urng_t& ranNumGen);
			void addConnectionGroup(ConnectionGroup* conGroup, nemo_network_t nemoNetwork, QHash<unsigned, synapse_id*>& volatileConGrpMap);
			void printConnection(unsigned source,unsigned targets[], unsigned delays[], float weights[], unsigned char is_plastic[], size_t length);
	};
}

#endif//NEMOLOADER_H
