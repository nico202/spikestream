#ifndef NRMEXCEPTION_H
#define NRMEXCEPTION_H

//SpikeStream includes
#include "SpikeStreamException.h"

//Other includes
#include <string>
using namespace std;

namespace spikestream {

	/*! Exception specific to database-related functionality. */
	class NRMException : public SpikeStreamException {
		public:
			NRMException(QString details);
			NRMException(string details);
			NRMException(const char* details);
			NRMException(const char* details, int data1);
			virtual ~NRMException();
	};

}


#endif//NRMEXCEPTION_H

