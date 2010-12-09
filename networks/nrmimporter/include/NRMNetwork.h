#ifndef NRMNETWORK_H
#define NRMNETWORK_H

//SpikeStream includes
#include "NRMConnection.h"
#include "NRMInputLayer.h"
#include "NRMNeuralLayer.h"

//Qt includes
#include <QHash>

namespace spikestream {

	/*! Holds information about an NRM network */
	class NRMNetwork {
		public:
			NRMNetwork();
			~NRMNetwork();
			void addInputLayer(int id, NRMInputLayer* inputLayer);
			void addNeuralLayer(int id, NRMNeuralLayer* neuralLayer);
			void createConnections();
			QList<NRMInputLayer*> getAllInputs();
			int getConfigVersion();
			int getInputLayerCount();
			int getNeuralLayerCount();
			int getNumberOfLayers() { return getInputLayerCount() + getNeuralLayerCount(); }
			QList<NRMInputLayer*> getPanelInputs();
			QList<NRMInputLayer*> getFramedImageInputs();
			NRMInputLayer* getInputById(int id);
			QList<NRMNeuralLayer*> getAllNeuralLayers();
			QList<NRMNeuralLayer*> getTrainedNeuralLayers();
			NRMLayer* getLayerById(int layerId, int objectType);
			NRMNeuralLayer* getNeuralLayerById(int id);
			void printConnections();
			void printInputLayers();
			void printNeuralLayers();
			void setConfigVersion(int version);


		private:
			//=========================  VARIABLES  ==============================
			/*! Version of the configuration file */
			int configVersion;

			/*! Holds information about the input layers */
			QHash<int, NRMInputLayer*> inputLayerMap;

			/*! Holds information about the neural layers */
			QHash<int, NRMNeuralLayer*> neuralLayerMap;

	};

}

#endif // NRMNETWORK_H
