#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "ArchiveInfo.h"
using namespace spikestream;

namespace spikestream {

    class Archive {
	public:
	    Archive(const ArchiveInfo& archiveInfo);
	    ~Archive();

	    int size() { return info.size(); }


	private:
	    //========================  VARIABLES  ===========================
	    /*! Class containing details about the archive */
	    ArchiveInfo info;

	    /*! The current time step */
	    unsigned int timeStep;
    };

}

#endif//ARCHIVE_H

