//SpikeStream includes
#include "NetworkDao.h"


/*! Constructor */
NetworkDao::NetworkDao(DBInfo& dbInfo) : AbstractDao(dbInfo){

}


/*! Destructor */
NetworkDao::~NetworkDao(){
}


/*! Returns the number of neurons in the specified volume. */
unsigned int NetworkDao::countNeuronsInVolume(SpikeStreamVolume& volume){
    checkDatabase();


    return 0;
}


