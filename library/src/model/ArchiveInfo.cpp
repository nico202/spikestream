#include "ArchiveInfo.h"
using namespace spikestream;


/*! Empty constructor */
ArchiveInfo::ArchiveInfo(){
    this->id = 0;
    this->networkID = 0;
    this->startDateTime = "";
    this->description = "";
    numberOfEntries = 0;
}


/*! Standard constructor */
ArchiveInfo::ArchiveInfo(unsigned int id, unsigned int networkID, const QString& startDateTime, const QString& description, int numberOfEntries){
	//Check that name and description will fit in the database
    if(description.size() > MAX_DATABASE_DESCRIPTION_LENGTH)
	throw SpikeStreamException("Archive description length exceeds maximum possible size in database.");

    this->id = id;
    this->networkID = networkID;
    this->startDateTime = startDateTime;
    this->description = description;
    this->numberOfEntries = numberOfEntries;
}


/*! Copy constructor */
ArchiveInfo::ArchiveInfo(const ArchiveInfo& archInfo){
    this->id = archInfo.id;
    this->networkID = archInfo.networkID;
    this->startDateTime = archInfo.startDateTime;
    this->description = archInfo.description;
    this->numberOfEntries = archInfo.numberOfEntries;
}


/*! Destructor */
ArchiveInfo::~ArchiveInfo(){
}


/*! Assignment operator */
ArchiveInfo& ArchiveInfo::operator=(const ArchiveInfo& rhs){
    //Check for self assignment
    if(this = &rhs)
	return this;

    this->id = rhs.id;
    this->networkID = rhs.networkID;
    this->startDateTime = rhs.startDateTime;
    this->description = rhs.description;
    this->numberOfEntries = rhs.numberOfEntries;
}


