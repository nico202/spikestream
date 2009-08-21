#include "NRMLayer.h"

NRMLayer::NRMLayer(){
}


NRMLayer::~NRMLayer(){
}


/*! Returns the number of neurons in the layer */
unsigned int NRMLayer::getSize(){
	return width * height;
}


