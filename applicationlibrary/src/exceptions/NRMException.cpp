//SpikeStream includes
#include "NRMException.h"
using namespace spikestream;

//Other includes
#include <sstream>
using namespace std;

/*! Constructor */
NRMException::NRMException(string msg){
	message = msg.data();
}


/*! Constructor */
NRMException::NRMException(const char* msg){
	message = msg;
}


/*! Constructor */
NRMException::NRMException(const char* msg, int data1){
	ostringstream errMsg;
	errMsg<<msg<<" "<<data1;
	message = errMsg.str();
}


/*! Destructor */
NRMException::~NRMException(void){
}


/*-------------------------------------------------------*/
/*-----                PUBLIC METHODS               -----*/
/*-------------------------------------------------------*/

/*! Returns information about the exception. */
const char* NRMException::getMessage(){
	return message.data();
}

