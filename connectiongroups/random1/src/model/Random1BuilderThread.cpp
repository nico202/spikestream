#include "Globals.h"
#include "NeuronGroup.h"
#include "Random1BuilderThread.h"
#include "SpikeStreamException.h"
using namespace spikestream;

/*! Constructor */
Random1BuilderThread::Random1BuilderThread(){
}


/*! Destructor */
Random1BuilderThread::~Random1BuilderThread(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Prepares class before it runs as a separate thread to add a neuron group */
void Random1BuilderThread::prepareAddConnectionGroup(const ConnectionGroupInfo& conGrpInfo){
	//Store information about the neuron group to be added
	this->connectionGroupInfo = conGrpInfo;

	//Check that the required parameters exist in the connection group and are in the appropriate range
	checkParameters();

	if(!Globals::networkLoaded())
		throw SpikeStreamException("Cannot add connection group - no network loaded.");
}


/*! Thread run method */
void Random1BuilderThread::run(){
	clearError();
	stopThread = false;
	newConnectionGroup = NULL;
	networkFinished = false;
	try{
		//Seed the random number generator
		srand(12345678);

		//Create network and archive dao for this thread
		threadNetworkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
		threadArchiveDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());
		Globals::getNetwork()->setNetworkDao(threadNetworkDao);
		Globals::getNetwork()->setArchiveDao(threadArchiveDao);

		//Add the connection group
		addConnectionGroup();

		//Wait for network to finish
		while(!networkFinished)
			msleep(250);

		//Reset network and archive daos in network
		Globals::getNetwork()->setNetworkDao(Globals::getNetworkDao());
		Globals::getNetwork()->setArchiveDao(Globals::getArchiveDao());

		//Clean up network and archive dao
		delete threadNetworkDao;
		delete threadArchiveDao;
	}
	catch (SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("An unknown error occurred.");
	}

	qDebug()<<"RANDOM1 BUILDER COMPLETE";
}


/*! Stops the thread running  */
void Random1BuilderThread::stop(){
	stopThread = true;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Called when network has finished adding the neuron groups */
void Random1BuilderThread::networkTaskFinished(){
	//Inform other classes that loading has completed
	emit progress(numberOfProgressSteps, numberOfProgressSteps);

	//Prevent this method being called when network finishes other tasks
	this->disconnect(Globals::getNetwork(), SIGNAL(taskFinished()));

	//Record that network has finished
	networkFinished = true;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds a neuron group with the specified parameters to the database */
void Random1BuilderThread::addConnectionGroup(){
	buildConnectionGroup();
	QList<ConnectionGroup*> conGrpList;
	conGrpList.append(newConnectionGroup);
	connect(Globals::getNetwork(), SIGNAL(taskFinished()), this, SLOT(networkTaskFinished()),  Qt::UniqueConnection);
	Globals::getNetwork()->addConnectionGroups(conGrpList);
}


/*! Clears error state and message */
void Random1BuilderThread::clearError(){
	error = false;
	errorMessage = "";
}


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
					weight = getRandomDouble(minWeightRange1, maxWeightRange1);
				else
					weight = getRandomDouble(minWeightRange2, maxWeightRange2);

				//Add the connection
				newConnectionGroup->addConnection(new Connection(fromIter.key(), toIter.key(), getRandomUInt(minDelay, maxDelay), weight, 0));
			}
		}

		//Update progress
		++cntr;
		emit progress(cntr, numberOfProgressSteps);
	}

	//qDebug()<<"STOP THREAD: "<<stopThread<<" EXPECTED NEURON GROUP SIZE: "<<(width*length*height)<<" PRE-DATABASE NEURON GROUP SIZE: "<<newNeuronGroup->size();
}


/*! Returns a parameter from the neuron group info parameter map checking that it actually exists */
double Random1BuilderThread::getParameter(const QString& paramName){
	QHash<QString, double> paramMap = connectionGroupInfo.getParameterMap();
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter with " + paramName + " does not exist in parameter map.");
	return paramMap[paramName];
}


/*! Returns a random number in the specified range */
double Random1BuilderThread::getRandomDouble(double min, double max){
	if(min > max)
		throw SpikeStreamException("Minimum cannot be greater than maximum");
	if(min == max)
		return min;
	double ranNum = (double)rand() / (double) RAND_MAX;
	return min + (max-min)*ranNum;
}


/*! Returns a random integer in the specified range */
unsigned int Random1BuilderThread::getRandomUInt(unsigned int min, unsigned int max){
	if(min > max)
		throw SpikeStreamException("Minimum cannot be greater than maximum");
	if(min == max)
		return min;
	return min + rand() % (max-min);
}


/*! Puts the builder into the error state with the provided error message */
void Random1BuilderThread::setError(const QString& errorMessage){
	error = true;
	this->errorMessage = errorMessage;
	stopThread = true;
	qDebug()<<"SETTING ERROR: "<<errorMessage;
}


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


