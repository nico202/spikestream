//SpikeStream includes
#include "SpikeStreamException.h"
#include "TestXMLParameterParser.h"
#include "XMLParameterParser.h"
using namespace spikestream;

#include <iostream>
using namespace std;

/*----------------------------------------------------------*/
/*-----                INIT AND CLEANUP                -----*/
/*----------------------------------------------------------*/

/*! Called after each test */
void TestXMLParameterParser::cleanup(){
}


/*! Called after all the tests */
void TestXMLParameterParser::cleanupTestCase() {
}


/*! Called before each test */
void TestXMLParameterParser::init(){
}


/*! Called before all the tests */
void TestXMLParameterParser::initTestCase(){
}


/*----------------------------------------------------------*/
/*-----                     TESTS                      -----*/
/*----------------------------------------------------------*/

void TestXMLParameterParser::testGetParameterMap(){
    QString xmlString = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
    xmlString += "<connection_group_parameters>";
    xmlString += "<parameter><name>param1</name>";
    xmlString += "<value>0.7</value>";
    xmlString += "</parameter>";
    xmlString += "<parameter><name>param2</name>";
    xmlString += "<value>20.7</value>";
    xmlString += "</parameter>";
    xmlString += "</connection_group_parameters>";

    XMLParameterParser parser;
    QHash<QString, double> paramMap;
    try{
	paramMap = parser.getParameterMap(xmlString);
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
    QVERIFY(paramMap.contains("param1"));
    QVERIFY(paramMap.contains("param2"));
    QCOMPARE(paramMap["param1"], 0.7);
    QCOMPARE(paramMap["param2"], 20.7);
}




