#include "Globals.h"

QString Globals::spikeStreamRoot = "";
QString Globals::workingDirectory = "";


/*---------------------------------------------------------------------------------*/
/*----------                  PUBLIC METHODS                          -------------*/
/*---------------------------------------------------------------------------------*/
/*! Returns the root directory, which should correspond to the SPIKESTREAM_ROOT
     environment variable.
*/
QString Globals::getSpikeStreamRoot(){
    return spikeStreamRoot;
}

QString Globals::getWorkingDirectory(){
    return workingDirectory;
}


/*---------------------------------------------------------------------------------*/
/*----------                  PRIVATE METHODS                         -------------*/
/*---------------------------------------------------------------------------------*/

/*! Sets the root directory. This function should only be accessed by friends of this
    class.
*/
void Globals::setSpikeStreamRoot(QString rootDir){
    spikeStreamRoot = rootDir;
}


void Globals::setWorkingDirectory(QString workDir){
    workingDirectory = workDir;
}



