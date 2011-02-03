//SpikeStream includes
#include "Pop1ExperimentManager.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
Pop1ExperimentManager::Pop1ExperimentManager() : SpikeStreamThread(){
}


/*! Destructor */
Pop1ExperimentManager::~Pop1ExperimentManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void Pop1ExperimentManager::run(){
	clearError();
	stopThread = false;

	try{
		for(int i=0; i<10 && !stopThread; ++i){
			qDebug()<<"Hello from pop 1 experiment: "<<i;
			msleep(400);
		}
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("Pop1ExperimentManager has thrown an unknown exception.");
	}

	stopThread = true;
}


/*! Sets up the experiment and starts thread running. */
void Pop1ExperimentManager::startExperiment(){
	start();
}

/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/
