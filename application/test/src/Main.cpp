//SpikeStream includes
#include "NRMConfigLoader.h"
#include "NRMException.h"

//Test includes
#include "TestRunner.h"

//Other includes
#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
    //Call the runner to invoke tests
    TestRunner::runTests();
    return 0;
}

