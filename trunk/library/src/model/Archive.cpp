#include "Archive.h"
using namespace spikestream;


/*! Constructor */
Archive::Archive(const ArchiveInfo& archiveInfo){
    this->info = archiveInfo;
    timeStep = 0;
}


/*! Destructor */
Archive::~Archive(){
}


