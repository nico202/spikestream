#include "NRMLayer.h"
using namespace spikestream;

/*! Constructor */
NRMLayer::NRMLayer(){
}


/*! Destructor */
NRMLayer::~NRMLayer(){
}


/*! Returns the number of neurons in the layer */
int NRMLayer::getSize(){
	return width * height;
}


