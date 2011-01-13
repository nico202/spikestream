//SpikeStream includes
#include "RasterModel.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <math.h>

/*! Constructor */
RasterModel::RasterModel(QList<NeuronGroup*>& neuronGroupList) : QwtRasterData(){
	//Find the maximum neuron ID
	maxNeuronID = 0;
	for(QList<NeuronGroup*>::iterator neurGrpIter = neuronGroupList.begin(); neurGrpIter != neuronGroupList.end(); ++neurGrpIter){
		maxNeuronID += (*neurGrpIter)->size();
	}

	//Initialize variables
	numTimeSteps = 1000;
	dataArray = new QHash<unsigned, bool>[numTimeSteps];
	dataCount = 0;
	readIndex = 0;
	writeIndex = 0;
	minTimeStep = 0;
}


/*! Destructor */
RasterModel::~RasterModel(){
	delete [] dataArray;
}


/*----------------------------------------------------------*/
/*------               PUBLIC METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Adds data to the model. This will erase the oldest bit of data if
	the time step is greater than the maximum. */
void RasterModel::addData(const QList<unsigned>& firingNeuronIDs, unsigned timeStep){
	qDebug()<<"Adding data: timestep="<<timeStep;

	//Fix minimum time step and reading location
	if(timeStep > (minTimeStep + numTimeSteps))
		minTimeStep = timeStep - numTimeSteps;
	readIndex = minTimeStep % numTimeSteps;

	//Clear data at write position
	dataArray[writeIndex].clear();

	//Add data to hash map at write index
	QList<unsigned>::const_iterator firingNeuronIDsEnd = firingNeuronIDs.end();
	for(QList<unsigned>::const_iterator neurIter = firingNeuronIDs.begin(); neurIter != firingNeuronIDsEnd; ++neurIter){
		for(QList<NeuronGroup*>::iterator neurGrpIter = neuronGroupList.begin(); neurGrpIter != neuronGroupList.end(); ++neurGrpIter){
			if((*neurGrpIter)->contains(*neurIter)){
				dataArray[writeIndex][*neurIter - (*neurGrpIter)->getStartNeuronID()] = true;//FIXME: ADD OFFSET FOR EACH NEURON GROUP
				break;
			}
		}
	}

	//Increase write index in a circular fashion
	++writeIndex;
	writeIndex %= numTimeSteps;
}


//Inherited from QwtData
QwtRasterData* RasterModel::copy() const{
	return (QwtRasterData*)this;
}


/*! Returns the minimum value of X */
int RasterModel::getMinX(){
	return minTimeStep;
}


/*! Returns the maximum value of X */
int RasterModel::getMaxX(){
	return minTimeStep + numTimeSteps;
}


/*! Returns the minimum value of Y */
int RasterModel::getMinY(){
	return 0;
}


/*! Returns the maximum value of Y */
int RasterModel::getMaxY(){
	return maxNeuronID;
}


//Inherited from QwtData
double RasterModel::value (double x, double y) const{
	int tmpX = (int)rint(x);
	int tmpY = (int)rint(y);

	int tmpIndx = (readIndex + tmpX) % numTimeSteps;
	qDebug()<<"Reading x="<<x<<"; y="<<y;

	if(dataArray[tmpIndx].contains(tmpY))
		return 1.0;
	return 0.0;
}


//Inherited from QwtData
QwtDoubleInterval RasterModel::range() const{
	return QwtDoubleInterval(minTimeStep, minTimeStep + numTimeSteps);
}



