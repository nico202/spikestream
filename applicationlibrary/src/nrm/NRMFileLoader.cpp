//SpikeStream includes
#include "NRMFileLoader.h"
#include "NRMException.h"
#include "NRMTrainingLoader.h"

//Other includes
#include <iostream>
using namespace std;

/*! Constructor */
NRMFileLoader::NRMFileLoader(){
    //Initialise variables
    stopThread = true;
    configFilePath = "";
    datasetFilePath = "";
    trainingFilePath = "";

    //Create config loader
    configLoader = new NRMConfigLoader();

    //Create dataset importer
    dataSetImporter = new NRMDataSetImporter();
}


/*! Destructor */
NRMFileLoader::~NRMFileLoader(){
    delete configLoader;
    delete dataSetImporter;
}



/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns the dataset that has been loaded */
NRMDataSet* NRMFileLoader::getDataSet(){
    if(!datasetLoaded)
	throw NRMException("No dataset loaded");
    return dataSetImporter->getDataSet();
}

/*! Returns the network that has been loaded */
NRMNetwork* NRMFileLoader::getNetwork(){
    if(!configLoaded)
	throw NRMException("No config loaded");
    return configLoader->getNetwork();
}


/*! Thread run method inherited from QThread.
    Loads up the configuration and  training files. */
void NRMFileLoader::run(){
    stopThread = false;
    configLoaded = false;
    datasetLoaded = false;
    trainingLoaded = false;
    clearError();

    if(configFilePath == "" || datasetFilePath == "" || trainingFilePath == ""){
	setError("Training or configuration file path(s) missing!");
	return;
    }

    try{
	//Load up configuration
	configLoader->reset();
	configLoader->loadConfig(configFilePath.toAscii());
	configLoaded = true;

	//Check to see if operation has been cancelled before loading training data
	if(stopThread)
	    return;

	//Load up training
	NRMTrainingLoader trainingLoader(configLoader->getNetwork());
	trainingLoader.loadTraining(trainingFilePath.toAscii());
	trainingLoaded = true;

	//Check to see if operation has been cancelled before loading training data
	if(stopThread)
	    return;

	//Load dataset
	dataSetImporter->reset();
	dataSetImporter->loadDataSet(datasetFilePath.toAscii());
	datasetLoaded = true;
    }
    catch(NRMException& ex){
	setError(ex.getMessage());
    }
    catch(...){
	setError("Unknown exception occurred.");
    }

    stopThread = true;
}


/*! Sets the file path to the configuration file */
void NRMFileLoader::setConfigFilePath(QString configFilePath){
    this->configFilePath = configFilePath;
}


/*! Sets the file path to the dataset file */
void NRMFileLoader::setDatasetFilePath(QString datasetFilePath){
    this->datasetFilePath = datasetFilePath;
}


/*! Sets the file path to the training file - this must match
    the configuration file or nasty errors occur. */
void NRMFileLoader::setTrainingFilePath(QString trainingFilePath){
    this->trainingFilePath = trainingFilePath;
}


/*! Stops the file loader */
void NRMFileLoader::stop(){
    stopThread = true;
}



/*! Clears the error message and state */
void NRMFileLoader::clearError(){
    error = false;
    errorMessage = "";
}

/*! Records that there has been an error and sets the thread to stop */
void NRMFileLoader::setError(const QString& errMsg){
    error = true;
    stopThread = true;
    errorMessage = errMsg;
}



