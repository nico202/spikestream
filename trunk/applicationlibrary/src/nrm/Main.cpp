#include "NRMConfigLoader.h"
#include "NRMException.h"

//Run the unit tests before starting
#define RUN_TESTS true

//Include appropriate header files for running tests
#ifdef RUN_TESTS
	#include "TestRunner.h"
#endif//RUN_TESTS


#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
	cout<<"Welcome to the NRM file loader!"<<endl;

	//Run any unit tests before launching application
	#ifdef RUN_TESTS
		TestRunner::runTests();
	#endif//RUN_TESTS

	return 0;
}

