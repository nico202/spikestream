#ifndef NRMFILELOADER_H
#define NRMFILELOADER_H

//SpikeStream includes
#include "NRMNetwork.h"
#include "NRMConfigLoader.h"
#include "NRMDataSet.h"
#include "NRMDataSetImporter.h"
using namespace spikestream;

//Qt includes
#include <QString>
#include <QThread>

namespace spikestream {

	/*! Loads up the configuration, dataset and training from NRM files.
		NOTE: Only works with the most recent and probably final version of NRM. */
	class NRMFileLoader : public QThread{
		public:
			NRMFileLoader();
			~NRMFileLoader();
			NRMDataSet* getDataSet();
			QString getErrorMessage() { return errorMessage; }
			NRMNetwork* getNetwork();
			bool isError() { return error; }
			void run();
			void setConfigFilePath(QString configFilePath);
			void setDatasetFilePath(QString configFilePath);
			void setTrainingFilePath(QString trainingFilePath);
			void stop();


		private:
			//========================  VARIABLES  =========================
			/*! Set to true to stop the thread running */
			bool stopThread;

			/*! Path to the NRM configuration file */
			QString configFilePath;

			/*! Path to the NRM configuration file */
			QString datasetFilePath;

			/*! Path to the training file */
			QString trainingFilePath;

			/*! Responsible for loading config file */
			NRMConfigLoader* configLoader;

			/*! Responsible for loading the dataset from file */
			NRMDataSetImporter* dataSetImporter;

			/*! Records if the configuration has been loaded. */
			bool configLoaded;

			/*! Records if the dataset has been loaded. */
			bool datasetLoaded;

			/*! Records if the training has been loaded. */
			bool trainingLoaded;

			/*! Flag set to true if an error has occurred. */
			bool error;

			/*! Message associated with an error */
			QString errorMessage;


			//=============================  METHODS  ==============================
			void clearError();
			void setError(const QString& errorMessage);
	};

}

#endif //NRMFILELOADER_H



