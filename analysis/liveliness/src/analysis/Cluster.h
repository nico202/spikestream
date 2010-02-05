#ifndef SUBSET_H
#define SUBSET_H

//Qt includes
#include <QList>
#include <QHash>

namespace spikestream {

	class Cluster {
		public:
			Cluster(unsigned int id, unsigned int timeStep, double liveliness, QHash<unsigned int, double>& neuronLivelinessMap);
			Cluster(unsigned int id, unsigned int timeStep, double liveliness, QList<unsigned int>& neuronIDList);
			Cluster(const Cluster& clstr);
			Cluster();
			~Cluster();
			Cluster& operator=(const Cluster& rhs);
			unsigned int getID() const { return id; }
			unsigned int getTimeStep() const { return timeStep; }
			double getLiveliness() const { return liveliness; }
			QList<unsigned int> getNeuronIDs() const;
			QHash<unsigned int, double>& getNeuronLivelinessMap() { return neuronLivelinessMap; }
			QString getNeuronIDString() const;


		private:
			//=====================  VARIABLES  ===================
			/*! ID of the cluster */
			unsigned int id;

			/*! Time step of the cluster */
			unsigned int timeStep;

			/*! The total liveliness associated with the cluster */
			double liveliness;

			/*! Map linking neuron IDs in the cluster with the colour with which
				they are to be displayed. */
			QHash<unsigned int, double> neuronLivelinessMap;

	};

}

#endif//SUBSET_H

