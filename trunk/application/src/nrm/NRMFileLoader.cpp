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
    trainingFilePath = "";

    //Create config loader
    configLoader = new NRMConfigLoader();
}


/*! Destructor */
NRMFileLoader::~NRMFileLoader(){
}

/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns the network that has been loaded */
NRMNetwork* NRMFileLoader::getNetwork(){
    return configLoader->getNetwork();
}


/*! Thread run method inherited from QThread.
    Loads up the configuration and  training files. */
void NRMFileLoader::run(){
    stopThread = false;

    if(configFilePath == "" || trainingFilePath == "")
	throw NRMException("Training or configuration file path(s) missing!");

    //Load up configuration
    configLoader->reset();
    configLoader->loadConfig(configFilePath.toAscii());

    //Check to see if operation has been cancelled before loading training data
    if(stopThread)
	return;

    //Load up training
    NRMTrainingLoader trainingLoader(configLoader->getNetwork());
    trainingLoader.loadTraining(trainingFilePath.toAscii());
}


/*! Sets the file path to the configuration file */
void NRMFileLoader::setConfigFilePath(QString configFilePath){
    this->configFilePath = configFilePath;
}


/*! Sets the file path to the training file - this must match
    the configuration file or nasty errors occur. */
void NRMFileLoader::setTrainingFilePath(QString trainingFilePath){
    this->trainingFilePath = trainingFilePath;
}


/*! Stops the file loader */
void NRMFileLoader::stop(){
    cout<<"STOPPING THREAD"<<endl;
    stopThread = true;
}





