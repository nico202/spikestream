#include "NetworkInfo.h"
#include "GlobalVariables.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Normal constructor */
NetworkInfo::NetworkInfo(unsigned int id, const QString& name, const QString& desc){
    //Check that name and description will fit in the database
    if(name.size() > MAX_DATABASE_NAME_LENGTH || description.size() > MAX_DATABASE_DESCRIPTION_LENGTH)
		throw SpikeStreamException("NeuralNetwork: Name and/or description length exceeds maximum possible size in database.");

    this->id = id;
    this->name = name;
    this->description = desc;
}


/*! Empty constructor */
NetworkInfo::NetworkInfo(){
    id = 0;
    name = "undefined";
    description = "undefined";
}


/*! Copy constructor */
NetworkInfo::NetworkInfo(const NetworkInfo& netInfo){
    this->id = netInfo.id;
    this->name = netInfo.name;
    this->description = netInfo.description;
}


/*! Destructor */
NetworkInfo::~NetworkInfo(){
}


/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */

/*! Assignment operator. */
NetworkInfo& NetworkInfo::operator=(const NetworkInfo& rhs) {
    // Check for self-assignment!
    if (this == &rhs)      // Same object?
		return *this;        // Yes, so skip assignment, and just return *this.

    this->id = rhs.id;
    this->name = rhs.name;
    this->description = rhs.description;

    return *this;
}


