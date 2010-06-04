#include "GlobalVariables.h"
#include "ArchiveInfo.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Empty constructor */
ArchiveInfo::ArchiveInfo(){
	reset();
}


/*! Standard constructor */
ArchiveInfo::ArchiveInfo(unsigned int id, unsigned int networkID, unsigned int unixTimestamp, const QString& description){
	//Check that name and description will fit in the database
    if(description.size() > MAX_DATABASE_DESCRIPTION_LENGTH)
	throw SpikeStreamException("Archive description length exceeds maximum possible size in database.");

    this->id = id;
    this->networkID = networkID;
    this->startDateTime = QDateTime::fromTime_t(unixTimestamp);
    this->description = description;
}


/*! Copy constructor */
ArchiveInfo::ArchiveInfo(const ArchiveInfo& archInfo){
    this->id = archInfo.id;
    this->networkID = archInfo.networkID;
    this->startDateTime = archInfo.startDateTime;
    this->description = archInfo.description;
}


/*! Destructor */
ArchiveInfo::~ArchiveInfo(){
}


/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                  -----*/
/*----------------------------------------------------------*/


/*! Assignment operator */
ArchiveInfo& ArchiveInfo::operator=(const ArchiveInfo& rhs){
    //Check for self assignment
    if(this == &rhs)
	return *this;

    this->id = rhs.id;
    this->networkID = rhs.networkID;
    this->startDateTime = rhs.startDateTime;
    this->description = rhs.description;
    return *this;
}

/*! Resets the class to default values */
void ArchiveInfo::reset(){
	this->id = 0;
	this->networkID = 0;
	this->startDateTime = QDateTime::fromTime_t(0);
	this->description = "Undescribed";
}


