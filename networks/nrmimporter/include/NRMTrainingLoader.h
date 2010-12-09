#ifndef NRMTRAININGLOADER_H
#define NRMTRAININGLOADER_H

//SpikeStream includes
#include "NRMNetwork.h"

namespace spikestream {

	/*! Loads up the weightless neuron training from an NRM file. */
	class NRMTrainingLoader {
		public:
			NRMTrainingLoader(NRMNetwork* network);
			~NRMTrainingLoader();
			NRMNetwork* getNetwork();
			void loadTraining(const char* filePath);
			void loadTrainingHeader(FILE* file, int numNetworkLayers, int& numTrainedLayers, int*& trainedLayerIDArray);
			void loadLayerTraining(int layerID, FILE* file);
			void printTrainingPattern(unsigned char* patternArray, unsigned int arraySize, int trainingStrNumber);

		private:
			//===========================  VARIABLES  ===============================
			/*! Network loaded from the configuration file */
			NRMNetwork* network;

			/*! The version of file */
			int loadVersion;

			/*! Record of the number of bytes read from file for debugging */
			int fileByteCount;


			//============================  METHODS  ================================
			void fReadFile(void* dataStruct, size_t sizeOfElement, size_t numElements, FILE * file );
	};

}

#endif // NRMTRAININGLOADER_H
