#ifndef NRMFILELOADER_H
#define NRMFILELOADER_H

//SpikeStream includes
#include "NRMNetwork.h"
#include "NRMConfigLoader.h"

//Qt includes
#include <QString>
#include <QThread>

class NRMFileLoader : public QThread{
    public:
	NRMFileLoader();
	~NRMFileLoader();
	NRMNetwork* getNetwork();
	void run();
	void setConfigFilePath(QString configFilePath);
	void setTrainingFilePath(QString trainingFilePath);
	void stop();

    private:
	//========================  VARIABLES  =========================
	/*! Set to true to stop the thread running */
	bool stopThread;

	/*! Path to the NRM configuration file */
	QString configFilePath;

	/*! Path to the training file */
	QString trainingFilePath;

	/*! Responsible for loading config file */
	NRMConfigLoader* configLoader;
};

#endif //NRMFILELOADER_H



