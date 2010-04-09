//SpikeStream includes
#include "Complex.h"
#include "Globals.h"
#include "StateBasedPhiSpectrogramData.h"
#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
StateBasedPhiSpectrogramData::StateBasedPhiSpectrogramData(const AnalysisInfo& analysisInfo) : AbstractSpectrogramData(analysisInfo){
	loadData();
}


/*! Destructor */
StateBasedPhiSpectrogramData::~StateBasedPhiSpectrogramData(){
}


/*----------------------------------------------------------*/
/*------               PUBLIC METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Inherited from QwtRasterData */
QwtRasterData* StateBasedPhiSpectrogramData::copy() const {
	return (QwtRasterData*)this;
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Loads up and processes the data */
void StateBasedPhiSpectrogramData::loadData(){
	reset();

	//Get the current list of complexes and a complete list of neuron ids
	StateBasedPhiAnalysisDao stateDao(Globals::getAnalysisDao()->getDBInfo());
	QList<Complex> newComplexList = stateDao.getComplexes(analysisInfo.getID());
	QList<unsigned int> networkNeurIDs = Globals::getNetworkDao()->getNeuronIDs(analysisInfo.getNetworkID());

	//Calculate the minimum and maximum neuron id for the graph range
	bool firstTime = true;
	foreach(unsigned int neurID, networkNeurIDs){
		if(firstTime){
			minNeuronID = neurID;
			maxNeuronID = neurID;
			firstTime = false;
		}
		else{
			if(neurID > maxNeuronID)
				maxNeuronID = neurID;
			if(neurID < minNeuronID)
				minNeuronID = neurID;
		}
	}

	//Set the bounding rectangle
	this->setBoundingRect(QwtDoubleRect(minNeuronID, minNeuronID, maxNeuronID-minNeuronID, maxNeuronID-minNeuronID));

	//Build complete data structure with all of the time steps in the analysis results
	//Also find the max phi to set the range
	firstTime = true;
	unsigned int oldTimeStep = 0;
	double maxPhi = 0.0;
	foreach(Complex cmplx, newComplexList){
		if(firstTime){
			addTimeStep(cmplx.getTimeStep(), networkNeurIDs);
			oldTimeStep = cmplx.getTimeStep();
			selectedTimeStep = cmplx.getTimeStep();
			firstTime = false;
		}
		else if(oldTimeStep != cmplx.getTimeStep()){
			addTimeStep(cmplx.getTimeStep(), networkNeurIDs);
			oldTimeStep = cmplx.getTimeStep();
		}

		if(cmplx.getPhi() > maxPhi)
			maxPhi = cmplx.getPhi();
	}

	//Find the maximum integration between each pair of neurons
	foreach(Complex cmplx, newComplexList){
		unsigned int timeStep = cmplx.getTimeStep();

		QList<unsigned int> tmpNeurIDs = cmplx.getNeuronIDs();
		for(int i=0; i<tmpNeurIDs.size(); ++i){
			for(int j=i; j<tmpNeurIDs.size(); ++j){
				if(cmplx.getPhi() > timeStepDataMap[timeStep][tmpNeurIDs.at(i)][tmpNeurIDs.at(j)])
					timeStepDataMap[timeStep][tmpNeurIDs.at(i)][tmpNeurIDs.at(j)] = cmplx.getPhi();
			}
		}
	}

	//Normalize the data
	for(QHash<unsigned int, QHash<unsigned int, QHash<unsigned int, double> > >::iterator tsIter = timeStepDataMap.begin(); tsIter != timeStepDataMap.end(); ++tsIter)	{
		for(QHash<unsigned int, QHash<unsigned int, double> >::iterator neur1Iter = tsIter.value().begin(); neur1Iter != tsIter.value().end(); ++neur1Iter){
			for(QHash<unsigned int, double>::iterator neur2Iter = neur1Iter.value().begin(); neur2Iter != neur1Iter.value().end(); ++neur2Iter){
				neur2Iter.value() /= maxPhi;
			}
		}
	}

	//Add map with average over all time steps
	addAllTimeStepsMap(networkNeurIDs);
}



