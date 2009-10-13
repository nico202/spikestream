#include "NRMDataSet.h"
using namespace spikestream;

#include <iostream>
using namespace std;

NRMDataSet::NRMDataSet(){
}


NRMDataSet::~NRMDataSet(){
    reset();
}


void NRMDataSet::addData(unsigned char* data){
    dataList.append(data);
}


void NRMDataSet::reset(){
    for(int i=0; i< dataList.size(); ++i)
	delete [] dataList[i];
    dataList.clear();

    width = -1;
    height = -1;
    colorPlanes = -1;
}

void NRMDataSet::print(){
    for(int i=1; i< dataList.size(); ++i){
	for(int h=0; h<height; ++h){
	    for(int w=0; w<width; ++w){
		if(dataList[i][h*width + w] == 7)
		    cout<<"1 ";
		else
		    cout<<"0 ";
	    }
	    cout<<endl;

	}
	cout<<endl;
    }
}

