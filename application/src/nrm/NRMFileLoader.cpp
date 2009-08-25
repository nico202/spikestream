//SpikeStream includes
#include "NRMFileLoader.h"
#include "NRMException.h"


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


NRMNetwork* NRMFileLoader::getNetwork(){
    return configLoader->getNetwork();
}


/*! Thread run method inherited from QThread */
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
}


void NRMFileLoader::setConfigFilePath(QString configFilePath){
    this->configFilePath = configFilePath;
}


void NRMFileLoader::setTrainingFilePath(QString trainingFilePath){
    this->trainingFilePath = trainingFilePath;
}


void NRMFileLoader::stop(){
    cout<<"STOPPING THREAD"<<endl;
    stopThread = true;
}





