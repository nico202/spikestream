#include "TestNRMDataSetImporter.h"
#include "NRMDataSetImporter.h"
using namespace spikestream;

void TestNRMDataSetImporter::cleanup(){
}


void TestNRMDataSetImporter::init(){
    workingDirectory = getenv("SPIKESTREAM_ROOT");
}


void TestNRMDataSetImporter::testLoadDataSet(){
    NRMDataSetImporter importer;
    QString fileName = workingDirectory + "/application/test/test_files/datasets/test2.set";

    importer.loadDataSet(fileName.toAscii());

    NRMDataSet* dataset = importer.getDataSet();
    QList<unsigned char*> dataList = dataset->getDataList();

    QCOMPARE(dataset->width, (int)4);
    QCOMPARE(dataset->height, (int)2);
    QCOMPARE(dataList.size(), (int)5);
    QCOMPARE((int)dataList[3][0], (int)7);
    QCOMPARE((int)dataList[3][1], (int)7);
    QCOMPARE((int)dataList[3][2], (int)0);
    QCOMPARE((int)dataList[3][3], (int)7);
    QCOMPARE((int)dataList[3][4], (int)0);
    QCOMPARE((int)dataList[3][5], (int)7);
    QCOMPARE((int)dataList[3][6], (int)0);
    QCOMPARE((int)dataList[3][7], (int)0);

}




