#include "testQApplication.h"
#include <iostream>
using namespace std;

/*! Constructor. */
testQApplication::testQApplication(int & argc, char ** argv) : QApplication(argc, argv){
    cout<<"here we are buddy"<<endl;
}

testQApplication::~testQApplication(){
}

