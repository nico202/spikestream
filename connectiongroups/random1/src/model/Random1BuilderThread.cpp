#include "Globals.h"
#include "NeuronGroup.h"
#include "Random1BuilderThread.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

/*! Constructor */
Random1BuilderThread::Random1BuilderThread() : AbstractConnectionBuilder() {
}


/*! Destructor */
Random1BuilderThread::~Random1BuilderThread(){
}


/*----------------------------------------------------------*/
/*-----                PROTECTED METHODS               -----*/
/*----------------------------------------------------------*/

/*! Returns a neuron group whose neurons are constructed according to the
	parameters in the neuron group info. */
void Random1BuilderThread::buildConnectionGroup(){
	//Sort out connection and neuron groups
	newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
	NeuronGroup* fromNeurGrp = Globals::getNetwork()->getNeuronGroup(connectionGroupInfo.getFromNeuronGroupID());
	NeuronGroup* toNeurGrp = Globals::getNetwork()->getNeuronGroup(connectionGroupInfo.getToNeuronGroupID());

	//Extract parameters
	double minWeightRange1 = getParameter("min_weight_range_1");
	double maxWeightRange1 = getParameter("max_weight_range_1");
	int percentWeightRange1 = (int)getParameter("percent_weight_range_1");
	double minWeightRange2 = getParameter("min_weight_range_2");
	double maxWeightRange2 = getParameter("max_weight_range_2");
	unsigned int minDelay = (unsigned int)getParameter("min_delay");
	unsigned int maxDelay = (unsigned int)getParameter("max_delay");
	double connectionProbability = getParameter("connection_probability");

	//Work through each pair of neurons
	numberOfProgressSteps = fromNeurGrp->size() + 1;
	int cntr = 0;
	double weight = 0;
	for(NeuronMap::iterator fromIter = fromNeurGrp->begin(); fromIter != fromNeurGrp->end(); ++fromIter){
		for(NeuronMap::iterator toIter = toNeurGrp->begin(); toIter != toNeurGrp->end(); ++toIter){
			//Decide if connection is made
			double ranNum = (double)rand() / (double)RAND_MAX;
			if(ranNum <= connectionProbability){
				//Calculate weight of connection
				if(rand() % 100 <= percentWeightRange1)
					weight = Util::getRandomDouble(minWeightRange1, maxWeightRange1);
				else
					weight = Util::getRandomDouble(minWeightRange2, maxWeightRange2);

				//Add the connection
				newConnectionGroup->addConnection(fromIter.key(), toIter.key(), Util::getRandomUInt(minDelay, maxDelay), weight);
			}
		}

		//Update progress
		++cntr;
		emit progress(cntr, numberOfProgressSteps, "Building connections...");
	}
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Extracts parameters from neuron group info and checks that they are in range. */
void Random1BuilderThread::checkParameters(){
	double minWeightRange1 = getParameter("min_weight_range_1");
	double maxWeightRange1 = getParameter("max_weight_range_1");
	int percentWeightRange1 = (int)getParameter("percent_weight_range_1");
	double minWeightRange2 = getParameter("min_weight_range_2");
	double maxWeightRange2 = getParameter("max_weight_range_2");
	unsigned int minDelay = (unsigned int)getParameter("min_delay");
	unsigned int maxDelay = (unsigned int)getParameter("max_delay");
	double connectionProbability = getParameter("connection_probability");

	if(minWeightRange1 > maxWeightRange1)
		throw SpikeStreamException("Min weight 1 range cannot be greater than max weight 1 range.");
	if(minWeightRange2 > maxWeightRange2)
		throw SpikeStreamException("Min weight 2 range cannot be greater than max weight 2 range.");
	if(percentWeightRange1 < 0 || percentWeightRange1 > 100)
		throw SpikeStreamException("Percent weight range 1 is out of range: " + QString::number(percentWeightRange1));
	if(minDelay > maxDelay)
		throw SpikeStreamException("Min delay cannot be greater than max delay.");
	if(connectionProbability < 0.0 || connectionProbability > 1.0)
		throw SpikeStreamException("Connection probability is out of range: " + QString::number(connectionProbability));
}


