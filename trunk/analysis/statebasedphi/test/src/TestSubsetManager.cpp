#include "TestSubsetManager.h"
#include "PhiUtil.h"
#include "SpikeStreamException.h"
#include "SubsetManager.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QHash>
#include <QDebug>


void TestSubsetManager::testBuildSubsetList(){
    //Create test class
    SubsetManager subsetManager;
    subsetManager.setPhiCalculator(new PhiCalculator());

    //Check that correct subsets were built
    try{
	//Build list of neurons to be converted into subsets
	QList<unsigned int> neuronIDList;
	neuronIDList.append(1);
	neuronIDList.append(2);
	neuronIDList.append(3);
	subsetManager.setNeuronIDList(neuronIDList);

	//Test class
	subsetManager.buildSubsetList();
	QList<Subset*> subsetList = subsetManager.getSubsetList();
	QCOMPARE(subsetList.size(), (int)4);
	QVERIFY(subsetContains(subsetList,"1,2"));
	QVERIFY(subsetContains(subsetList,"2,3"));
	QVERIFY(subsetContains(subsetList,"1,3"));
	QVERIFY(subsetContains(subsetList,"1,2,3"));

	//Build list of neurons to be converted into subsets
	neuronIDList.clear();
	for(unsigned int i=20; i<29; ++i)
	    neuronIDList.append(i);
	subsetManager.setNeuronIDList(neuronIDList);

	//Test class
	subsetManager.buildSubsetList();
	subsetList = subsetManager.getSubsetList();
	QCOMPARE(subsetList.size(), (int)502);
	QVERIFY(subsetContains(subsetList,"20,22,24,26,28"));
	QVERIFY(subsetContains(subsetList,"20,21,22,23,24,25,26,27,28"));
	QVERIFY(subsetContains(subsetList,"27,28"));
	QVERIFY(subsetContains(subsetList,"21,28"));
    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
}


void TestSubsetManager::testIdentifyComplexes(){
    //Create test class
    SubsetManager subsetManager;
    PhiCalculator* phiCalc = PhiUtil::buildPhiTestNetwork1();
    subsetManager.setPhiCalculator(phiCalc);
    StateBasedPhiAnalysisDaoDuck stateDaoDuck;
    subsetManager.setStateDao(&stateDaoDuck);
    QList<unsigned int> neuronIDList;

    //Check that correct subsets were built
    try{
	//Add test network 1 neuron ids and run analysis
	for(unsigned int i=1; i<=4; ++i)
	    neuronIDList.append(i);
	subsetManager.setNeuronIDList(neuronIDList);
	subsetManager.buildSubsetList();
	subsetManager.calculateSubsetsPhi();
	subsetManager.identifyComplexes();

	//Check complexes are correct
	QList<Complex> complexList = stateDaoDuck.getComplexList();
	QVERIFY( complexExists(complexList, "1,2", 2.0) );
	QVERIFY( complexExists(complexList, "3,4", 2.0) );

	//Clean up
	delete phiCalc;

	//Set up test class for network 2
	PhiCalculator* phiCalc = PhiUtil::buildPhiTestNetwork2();
	subsetManager.setPhiCalculator(phiCalc);
	stateDaoDuck.reset();
	neuronIDList.clear();
	for(unsigned int i=1; i<=6; ++i)
	    neuronIDList.append(i);
	subsetManager.setNeuronIDList(neuronIDList);
	subsetManager.buildSubsetList();
	subsetManager.calculateSubsetsPhi();
	subsetManager.identifyComplexes();

	//Check complexes exist
	complexList = stateDaoDuck.getComplexList();
	QVERIFY( complexExists(complexList, "1,2,3", 3.0) );
	QVERIFY( complexExists(complexList, "3,6", 1.0) );
	QVERIFY( complexExists(complexList, "2,5", 1.0) );
	QVERIFY( complexExists(complexList, "1,4", 1.0) );

	//Don't clean up most recent phi calculator because it is deleted by the subset manager

    }
    catch(SpikeStreamException& ex){
	QFAIL(ex.getMessage().toAscii());
    }
}


/*-------------------------------------------------------------*/
/*-------                 PRIVATE METHODS                ------*/
/*-------------------------------------------------------------*/

/*! Checks the list of complexes to see if it contains the specified complex */
bool TestSubsetManager::complexExists(QList<Complex>& complexList, const QString neurIDStr, double phi){
    //Convert neuron IDS to a map
    QHash<unsigned int, bool> neuronIDMap;
    QStringList neurIDStrList = neurIDStr.split(",");
    foreach(QString string, neurIDStrList)
	neuronIDMap[Util::getUInt(string)] = true;

    //Work through complexes
    foreach(Complex complex, complexList){
	//Check phi and neuron ids
	//Ignore subsets that are the wrong size
	if( (complex.getPhi() == phi) && (complex.getNeuronIDs().size() == neuronIDMap.size()) ){
	    //Check that all of the neurons in the subset are in the map
	    bool match = true;
	    QList<unsigned int> neurIDList = complex.getNeuronIDs();
	    foreach(unsigned int cmplxNeurID, neurIDList){
		if(!neuronIDMap.contains(cmplxNeurID)){
		    match = false;
		    break;
		}
	    }
	    if(match)
		return true;
	}
    }
    return false;
}


/*! Checks that the comma separated list of neuron ids can be found in one subset */
bool TestSubsetManager::subsetContains(QList<Subset*>& subsetList, const QString neuronIDs){
    //Convert neuron IDS to a map
    QHash<unsigned int, bool> neuronIDMap;
    QStringList neurIDStrList = neuronIDs.split(",");
    foreach(QString string, neurIDStrList)
	neuronIDMap[Util::getUInt(string)] = true;

    //Work through subsets
    foreach(Subset* subset, subsetList){
	//Ignore subsets that are the wrong size
	if(subset->size() == neuronIDMap.size()){
	    //Check that all of the neurons in the subset are in the map
	    QList<unsigned int> subNeurIDs = subset->getNeuronIDs();
	    bool match = true;
	    foreach(unsigned int subNeurID, subNeurIDs){
		if(!neuronIDMap.contains(subNeurID)){
		    match = false;
		    break;
		}
	    }
	    if(match)
		return true;
	}
    }
    return false;
}


