//SpikeStream includes
#include "Globals.h"
#include "LivelinessSpectrogramData.h"
#include "LivelinessDao.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
LivelinessSpectrogramData::LivelinessSpectrogramData(const AnalysisInfo& analysisInfo) : AbstractSpectrogramData(analysisInfo){
	loadData();
}


/*! Destructor */
LivelinessSpectrogramData::~LivelinessSpectrogramData(){
}


/*----------------------------------------------------------*/
/*------               PUBLIC METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Inherited from QwtRasterData */
QwtRasterData* LivelinessSpectrogramData::copy() const {
	return (QwtRasterData*)this;
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Loads up and processes the data */
void LivelinessSpectrogramData::loadData(){
	reset();

	//Get the current list of complexes and a complete list of neuron ids
	LivelinessDao livelinessDao(Globals::getAnalysisDao()->getDBInfo());
	QList<Cluster> newClusterList = livelinessDao.getClusters(analysisInfo.getID());
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
	double maxLiveliness = 0.0;
	foreach(Cluster clstr, newClusterList){
		if(firstTime){
			addTimeStep(clstr.getTimeStep(), networkNeurIDs);
			oldTimeStep = clstr.getTimeStep();
			selectedTimeStep = clstr.getTimeStep();
			firstTime = false;
		}
		else if(oldTimeStep != clstr.getTimeStep()){
			addTimeStep(clstr.getTimeStep(), networkNeurIDs);
			oldTimeStep = clstr.getTimeStep();
		}

		if(clstr.getLiveliness() > maxLiveliness)
			maxLiveliness = clstr.getLiveliness();
	}

	//Find the maximum integration between each pair of neurons
	foreach(Cluster clstr, newClusterList){
		unsigned int timeStep = clstr.getTimeStep();

		/* Get neurons in the cluster
			NOTE: Neuron ids are in an arbitrary order and this is unlikely to be the
			same order as the list of neuron ids from the database */
		QList<unsigned int> tmpNeurIDs = clstr.getNeuronIDs();

		//Work through all possible combinations of the neurons. Current measure is symmetrical
		for(int i=0; i<tmpNeurIDs.size(); ++i){
			for(int j=i; j<tmpNeurIDs.size(); ++j){
				unsigned int tmpNeur1 = tmpNeurIDs.at(i);
				unsigned int tmpNeur2 = tmpNeurIDs.at(j);

				if(tmpNeur1 <= tmpNeur2){
					if(!timeStepDataMap[timeStep].contains(tmpNeur1) || !timeStepDataMap[timeStep][tmpNeur1].contains(tmpNeur2))
						qCritical()<<"Neuron ID Key error";
					if(clstr.getLiveliness() > timeStepDataMap[timeStep][tmpNeur1][tmpNeur2])
						timeStepDataMap[timeStep][tmpNeur1][tmpNeur2] = clstr.getLiveliness();
				}
				else{
					if(!timeStepDataMap[timeStep].contains(tmpNeur2) || !timeStepDataMap[timeStep][tmpNeur2].contains(tmpNeur1))
						qCritical()<<"Neuron ID Key error";
					if(clstr.getLiveliness() > timeStepDataMap[timeStep][tmpNeur2][tmpNeur1])
						timeStepDataMap[timeStep][tmpNeur2][tmpNeur1] = clstr.getLiveliness();
				}
			}
		}
	}

	//Normalize the data
	for(QHash<unsigned int, QHash<unsigned int, QHash<unsigned int, double> > >::iterator tsIter = timeStepDataMap.begin(); tsIter != timeStepDataMap.end(); ++tsIter)	{
		for(QHash<unsigned int, QHash<unsigned int, double> >::iterator neur1Iter = tsIter.value().begin(); neur1Iter != tsIter.value().end(); ++neur1Iter){
			for(QHash<unsigned int, double>::iterator neur2Iter = neur1Iter.value().begin(); neur2Iter != neur1Iter.value().end(); ++neur2Iter){
				neur2Iter.value() /= maxLiveliness;
			}
		}
	}

	//Add map with average over all time steps
	addAllTimeStepsMap(networkNeurIDs);
}



