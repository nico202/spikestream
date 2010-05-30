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

namespace spikestream {

	/*! Loads the network into the graphics hardware ready to run with Nemo */
	class NemoLoader : public QObject {
		Q_OBJECT

		public:
			NemoLoader();
			~NemoLoader();
			void loadSimulation(Network* network, const bool* stop);

		signals:
			void progress(int stepsCompleted, int totalSteps);

		private:
			//======================  VARIABLES  =======================
			/*! Network to be loaded into the  */
			NetworkDao* networkDao;

			/*! Dao for the archive database - pass to class to enable it to run as a separate thread */
			ArchiveDao* archiveDao;


	};
}

#endif//NEMOLOADER_H
