#include "TestNRMDataImporter.h"
using namespace spikestream;

void TestNRMDataImporter::cleanup(){
}


void TestNRMDataImporter::init(){
	workingDirectory = QCoreApplication::applicationDirPath();
	workingDirectory.truncate(workingDirectory.size() - 4);//Trim the "/bin" off the end
	workingDirectory += "/networks/nrmimporter/test/test_files/";
}



void TestNRMDataImporter::testAddTraining(){
}

