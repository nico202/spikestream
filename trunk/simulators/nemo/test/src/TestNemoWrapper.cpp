#include "TestNemoWrapper.h"
#include "NemoWrapper.h"
using namespace spikestream;


void TestNemoWrapper::testConstructor(){
	NemoWrapper* nemoWrapper = new NemoWrapper();
	delete nemoWrapper;
}


