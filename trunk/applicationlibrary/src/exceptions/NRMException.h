#ifndef NRMEXCEPTION_H
#define NRMEXCEPTION_H

//Other includes
#include <string>
using namespace std;


class NRMException{
	public:
		NRMException(string msg);
		NRMException(const char* msg);
		NRMException(const char* msg, int data1);
		virtual ~NRMException(void);
		const char* getMessage();

	private:
		string message;
};


#endif//NRMEXCEPTION_H
