#ifndef NRMCONFIGLOADER_H
#define NRMCONFIGLOADER_H

//SpikeStream includes
#include "NRMNetwork.h"

//Qt includes
#include <QHash>

//Other includes
#include <string>
using namespace std;

namespace spikestream {

	/*! Loads up information from an NRM configuration file. File should have extension .cfg */
	class NRMConfigLoader{
		public:
			NRMConfigLoader(void);
			~NRMConfigLoader(void);
			void loadConfig(const char* filePath);
			int getOpVer(){ return 2; }//Returns the current version of NRM
			NRMNetwork* getNetwork();
			void reset();


		private:
			//=========================  VARIABLES  ==============================
			/*! The network that is loaded from the configuration file */
			NRMNetwork* network;

			/*! Count of the bytes read from the file */
			int fileByteCount;

			//==========================  METHODS  ===============================
			void fReadFile(void* dataStruct, size_t sizeOfElement, size_t numElements, FILE* file );
			void loadConfigVersion(FILE* file);
			void loadConnectionParameters(conType& conParams, FILE* file);
			void loadConnections(FILE* file);
			void loadInputs(FILE* file);
			void loadNeuralLayers(FILE* file);

	};

}

#endif//NRMCONFIGLOADER_H

