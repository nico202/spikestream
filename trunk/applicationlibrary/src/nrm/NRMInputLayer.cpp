#include "NRMInputLayer.h"
using namespace spikestream;

#include <iostream>
using namespace std;

/*! Constructor */
NRMInputLayer::NRMInputLayer(){
    type = NRM_INPUT_LAYER;
}

/*! Destructor */
NRMInputLayer::~NRMInputLayer(){
}


/*! Prints out information about the layer. */
void NRMInputLayer::print(){
	cout<<"   Name: "<<frameName<<endl;
	cout<<"   WinType: "<<winType<<endl;
	cout<<"   Width: "<<width<<endl;
	cout<<"   Height: "<<height<<endl;
	cout<<"   Width2: "<<width2<<endl;
	cout<<"   Height2: "<<height2<<endl;
	cout<<"   x: "<<x<<endl;
	cout<<"   y: "<<y<<endl;
	cout<<"   r: "<<r<<endl;
	cout<<"   b: "<<b<<endl;
	cout<<"   ColPlanes: "<<colPlanes<<endl;
}

