#ifndef NEMOLOADER_H
#define NEMOLOADER_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "Network.h"
using namespace spikestream;

//Nemo includes
#include "nemo.hpp"

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
			nemo::Network* buildNemoNetwork(Network* network, const bool* stop);

		signals:
			void progress(int stepsCompleted, int totalSteps);

		private:
			//======================  VARIABLES  =======================
			/*! Network to be loaded into the  */
			NetworkDao* networkDao;

			/*! Dao for the archive database - pass to class to enable it to run as a separate thread */
			ArchiveDao* archiveDao;


			//======================  METHODS  =======================
			void addExcitatoryNeuronGroup(NeuronGroup* neuronGroup, nemo::Network* nemoNet, urng_t& ranNumGen);
			void addInhibitoryNeuronGroup(NeuronGroup* neuronGroup, nemo::Network* nemoNet, urng_t& ranNumGen);
			void addConnectionGroup(ConnectionGroup* conGroup, nemo::Network* nemoNet);
	};
}

#endif//NEMOLOADER_H
