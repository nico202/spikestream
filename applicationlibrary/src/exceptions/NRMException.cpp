//SpikeStream includes
#include "NRMException.h"
using namespace spikestream;

//Other includes
#include <sstream>
using namespace std;

NRMException::NRMException(string msg){
	message = msg.data();
}

NRMException::NRMException(const char* msg){
	message = msg;
}

NRMException::NRMException(const char* msg, int data1){
	ostringstream errMsg;
	errMsg<<msg<<" "<<data1;
	message = errMsg.str();
}

NRMException::~NRMException(void){
}

const char* NRMException::getMessage(){
	return message.data();
}
