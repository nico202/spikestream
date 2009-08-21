#include "NRMInputLayer.h"

#include <iostream>
using namespace std;

NRMInputLayer::NRMInputLayer(){
}

NRMInputLayer::~NRMInputLayer(){
}


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

