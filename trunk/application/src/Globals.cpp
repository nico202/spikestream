#include "Globals.h"

QString Globals::rootDirectory = "";


/*---------------------------------------------------------------------------------*/
/*----------                  PUBLIC METHODS                          -------------*/
/*---------------------------------------------------------------------------------*/
/*! Returns the root directory, which should correspond to the SPIKESTREAM_ROOT
     environment variable.
*/
QString Globals::getRootDirectory(){
    return rootDirectory;
}


/*---------------------------------------------------------------------------------*/
/*----------                  PRIVATE METHODS                         -------------*/
/*---------------------------------------------------------------------------------*/

/*! Sets the root directory. This function should only be accessed by friends of this
    class.
*/
void Globals::setRootDirectory(QString rootDir){
    rootDirectory = rootDir;
}



