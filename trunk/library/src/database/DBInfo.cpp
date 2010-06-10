#include "DBInfo.h"
using namespace spikestream;

//Initialize static varaibles
QString DBInfo::UNDEFINED = "undefined";


/*! Normal constructor */
DBInfo::DBInfo(QString host, QString user, QString password, QString database){
    this->host = host;
    this->database = database;
    this->password = password;
    this->user = user;
}


/*! Empty constructor */
DBInfo::DBInfo(){
	host = DBInfo::UNDEFINED;
	database = DBInfo::UNDEFINED;
	password = DBInfo::UNDEFINED;
	user = DBInfo::UNDEFINED;
}


/*! Copy constructor */
DBInfo::DBInfo(const DBInfo& dbInfo){
    this->host = dbInfo.host;
    this->database = dbInfo.database;
    this->password = dbInfo.password;
    this->user = dbInfo.user;
}


/*! Destructor */
DBInfo::~DBInfo(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Assignment operator. */
DBInfo& DBInfo::operator=(const DBInfo &rhs) {
    // Check for self-assignment!
    if (this == &rhs)      // Same object?
      return *this;        // Yes, so skip assignment, and just return *this.

    this->host = rhs.host;
    this->database = rhs.database;
    this->password = rhs.password;
    this->user = rhs.user;

    return *this;
}


/*! Returns a string representation of the database information */
QString DBInfo::toString() const{
    QString tmpStr;
    tmpStr += "host=" + host + "; ";
    tmpStr += "user=" + user + "; ";
    tmpStr += "password=" + password + "; ";
    tmpStr += "database=" + database+ "; ";
    return tmpStr;
}
