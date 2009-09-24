#include "NRMLayer.h"
using namespace nrm;

NRMLayer::NRMLayer(){
}


NRMLayer::~NRMLayer(){
}


/*! Returns the number of neurons in the layer */
int NRMLayer::getSize(){
	return width * height;
}


