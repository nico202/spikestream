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

	bool configLoaded;
	bool datasetLoaded;
	bool trainingLoaded;

	bool error;
	QString errorMessage;

	//=============================  METHODS  ==============================
	void clearError();
	void setError(const QString& errorMessage);
};

#endif //NRMFILELOADER_H



