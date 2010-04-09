//SpikeStream includes
#include "Globals.h"
#include "AbstractSpectrogramData.h"
#include "SpikeStreamAnalysisException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
AbstractSpectrogramData::AbstractSpectrogramData(const AnalysisInfo& analysisInfo) : QwtRasterData(){
	this->analysisInfo = analysisInfo;
	allTimeSteps = true;
	showNeuronIDs = true;
}


/*! Destructor */
AbstractSpectrogramData::~AbstractSpectrogramData(){

}


/*----------------------------------------------------------*/
/*------               PUBLIC METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Inherited from QwtRasterData */
QwtRasterData* AbstractSpectrogramData::copy() const {
	return (QwtRasterData*)this;
}


/*! Inherited from QwtRasterData */
QwtDoubleInterval AbstractSpectrogramData::range() const {
	return QwtDoubleInterval(0.0, 1.0);
}


/*! Sets the current time step that is being plotted */
void AbstractSpectrogramData::showSelectedTimeStep(unsigned int timeStep){
	selectedTimeStep = timeStep;
}


/*! Controls whether the average over all time steps is shown */
void AbstractSpectrogramData::showAllTimeSteps(bool show){
	allTimeSteps = show;
}


/*! Controls whether neuron ids or the numbers 1,2,...*/
void AbstractSpectrogramData::setShowNeuronIDs(bool show){
	showNeuronIDs = show;
	if(showNeuronIDs){
		this->setBoundingRect(QwtDoubleRect(minNeuronID, minNeuronID, maxNeuronID-minNeuronID, maxNeuronID-minNeuronID));
	}
	else
		this->setBoundingRect(QwtDoubleRect(1, 1, maxNeuronID-minNeuronID, maxNeuronID-minNeuronID));
}


/*! Inherited from QwtRasterData */
double AbstractSpectrogramData::value(double x, double y) const {
	unsigned int tmpX = (unsigned int)rint(x);
	unsigned int tmpY = (unsigned int)rint(y);

	//Resolve data as neuron ids or as numbers from 1-networksize
	if(!showNeuronIDs){
		tmpX += minNeuronID - 1;
		tmpY += minNeuronID - 1;
	}

	//Get time step
	unsigned int timeStep = selectedTimeStep;
	if(allTimeSteps)
		timeStep = allTimeStepsKey;

	//Check data exists for this time step
	if(!timeStepDataMap.contains(timeStep)){
		qCritical()<<"Time step cannot be found"<<timeStep;
		return 0.0;
	}

	if(tmpX > tmpY){
		if(!timeStepDataMap[timeStep].contains(tmpY) || !timeStepDataMap[timeStep][tmpY].contains(tmpX)){
			qCritical()<<"Y and X value cannot be found."<<tmpY<<" "<<tmpX;
			return 0.0;
		}
		return timeStepDataMap[timeStep][tmpY][tmpX];
	}

	if(!timeStepDataMap[timeStep].contains(tmpX) || !timeStepDataMap[timeStep][tmpX].contains(tmpY)){
		qCritical()<<"X and Y value cannot be found."<<tmpX<<" "<<tmpY;
		return 0.0;
	}
	return timeStepDataMap[timeStep][tmpX][tmpY];
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Adds a time step to the data map with entries for all the neuron ids */
void AbstractSpectrogramData::addTimeStep(unsigned int timeStep, const QList<unsigned int>& neurIDList){
	if(timeStepDataMap.contains(timeStep))
		throw SpikeStreamAnalysisException("Time step already exists in time step data map.");

	for(int i=0; i<neurIDList.size(); ++i){
		for(int j=i; j<neurIDList.size(); ++j){
			timeStepDataMap[timeStep][neurIDList.at(i)][neurIDList.at(j)] = 0.0;
		}
	}
}


/*! Calculates the average over all time steps */
void AbstractSpectrogramData::addAllTimeStepsMap(const QList<unsigned int>& neurIDList){
	//Identify a key that is not currently in use
	for(int i=0; i< timeStepDataMap.size() + 10; ++i){
		if(!timeStepDataMap.contains(i)){
			allTimeStepsKey = i;
			addTimeStep(i, neurIDList);
			break;
		}
	}

	//Sum the integration between neurons for each time step
	for(QHash<unsigned int, QHash<unsigned int, QHash<unsigned int, double> > >::iterator tsIter = timeStepDataMap.begin(); tsIter != timeStepDataMap.end(); ++tsIter)	{
		if(tsIter.key() != allTimeStepsKey){
			for(QHash<unsigned int, QHash<unsigned int, double> >::iterator neur1Iter = tsIter.value().begin(); neur1Iter != tsIter.value().end(); ++neur1Iter){
				for(QHash<unsigned int, double>::iterator neur2Iter = neur1Iter.value().begin(); neur2Iter != neur1Iter.value().end(); ++neur2Iter){
					timeStepDataMap[allTimeStepsKey][neur1Iter.key()][neur2Iter.key()] += neur2Iter.value();
				}
			}
		}
	}

	//Divide by the number of time steps
	double numTimeSteps = timeStepDataMap.size() - 1;
	double max = 0.0;
	for(QHash<unsigned int, QHash<unsigned int, double> >::iterator neur1Iter = timeStepDataMap[allTimeStepsKey].begin(); neur1Iter != timeStepDataMap[allTimeStepsKey].end(); ++neur1Iter){
		for(QHash<unsigned int, double>::iterator neur2Iter = neur1Iter.value().begin(); neur2Iter != neur1Iter.value().end(); ++neur2Iter){
			timeStepDataMap[allTimeStepsKey][neur1Iter.key()][neur2Iter.key()] /= numTimeSteps;

			//Find maximum
			if(timeStepDataMap[allTimeStepsKey][neur1Iter.key()][neur2Iter.key()] > max)
				max = timeStepDataMap[allTimeStepsKey][neur1Iter.key()][neur2Iter.key()];
		}
	}

	//Normalize
	for(QHash<unsigned int, QHash<unsigned int, double> >::iterator neur1Iter = timeStepDataMap[allTimeStepsKey].begin(); neur1Iter != timeStepDataMap[allTimeStepsKey].end(); ++neur1Iter){
		for(QHash<unsigned int, double>::iterator neur2Iter = neur1Iter.value().begin(); neur2Iter != neur1Iter.value().end(); ++neur2Iter){
			timeStepDataMap[allTimeStepsKey][neur1Iter.key()][neur2Iter.key()] /= max;
		}
	}
}


/*! Prints out the data */
void AbstractSpectrogramData::printData(){
	for(QHash<unsigned int, QHash<unsigned int, QHash<unsigned int, double> > >::iterator tsIter = timeStepDataMap.begin(); tsIter != timeStepDataMap.end(); ++tsIter)	{
		if(tsIter.key() == allTimeStepsKey)
			cout<<"-------------  All Time Steps  -----------------"<<endl;
		else
			cout<<"-------------  Time step: "<<tsIter.key()<<"  -----------------"<<endl;
		for(QHash<unsigned int, QHash<unsigned int, double> >::iterator neur1Iter = tsIter.value().begin(); neur1Iter != tsIter.value().end(); ++neur1Iter){
			for(QHash<unsigned int, double>::iterator neur2Iter = neur1Iter.value().begin(); neur2Iter != neur1Iter.value().end(); ++neur2Iter){
				cout<<"From "<<neur1Iter.key()<<" to "<<neur2Iter.key()<<" phi="<<neur2Iter.value()<<endl;
			}
		}
	}
	cout<<endl;
}


/*! Resets the data */
void AbstractSpectrogramData::reset(){
	timeStepDataMap.clear();
}


