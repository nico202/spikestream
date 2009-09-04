#include "NetworkInfo.h"
using namespace spikestream;

/*! Normal constructor */
NetworkInfo::NetworkInfo(unsigned int id, const QString& name, bool locked){
    this->id = id;
    this->name = name;
    this->locked = locked;
}


/*! Empty constructor */
NetworkInfo::NetworkInfo(){
    id = 0;
    name = "undefined";
    locked = 0;
}


/*! Copy constructor */
NetworkInfo::NetworkInfo(const NetworkInfo& netInfo){
    this->id = netInfo.id;
    this->name = netInfo.name;
    this->locked = netInfo.locked;
}


/*! Destructor */
NetworkInfo::~NetworkInfo(){
}


/*! Assignment operator. */
NetworkInfo& NetworkInfo::operator=(const NetworkInfo& rhs) {
    // Check for self-assignment!
    if (this == &rhs)      // Same object?
      return *this;        // Yes, so skip assignment, and just return *this.

    this->id = rhs.id;
    this->name = rhs.name;
    this->locked = rhs.locked;

    return *this;
}


