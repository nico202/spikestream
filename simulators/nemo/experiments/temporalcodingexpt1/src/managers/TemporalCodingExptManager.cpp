//SpikeStream includes
#include "Globals.h"
#include "TemporalCodingExptManager.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
TemporalCodingExptManager::TemporalCodingExptManager() : SpikeStreamThread(){
}


/*! Destructor */
TemporalCodingExptManager::~TemporalCodingExptManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void TemporalCodingExptManager::run(){
	clearError();
	stopThread = false;
	unsigned origWaitInterval = nemoWrapper->getWaitInterval_ms();
	nemoWrapper->setWaitInterval(1);//Minimal wait between steps

	try{
		//Seed the random number generator
		Util::seedRandom(randomSeed);

		//Start appropriate experiment
		emit statusUpdate("Starting Experiment " + QString::number(experimentNumber + 1));
		switch(experimentNumber){
			case EXPERIMENT1:
				runExperiment1();
			break;
			case EXPERIMENT2:
				runExperiment2();
			break;
			case EXPERIMENT3:
				runExperiment3();
			break;
			case EXPERIMENT4:
				runExperiment4();
			break;
			case EXPERIMENT5:
				runExperiment5();
			break;
			default:
				throw SpikeStreamException("Experiment number not recognized: " + QString::number(experimentNumber));
		}
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("TemporalCodingExptManager has thrown an unknown exception.");
	}

	emit statusUpdate("Experiment complete.");
	nemoWrapper->setWaitInterval(origWaitInterval);//Restore wrapper to original state
	experimentNumber = NO_EXPERIMENT;
	stopThread = true;
}


/*! Sets up the experiment and starts thread running. */
void TemporalCodingExptManager::startExperiment(NemoWrapper* nemoWrapper, QHash<QString, double>& parameterMap){
	this->nemoWrapper = nemoWrapper;
	storeParameters(parameterMap);

	//Get neuron layers that we need
	QList<NeuronGroup*> neurGrpList = Globals::getNetwork()->getNeuronGroups();
	inputNeuronGroup = NULL, featureNeuronGroup = NULL;
	outputNeuron1ID = 0, outputNeuron2ID = 0;
	foreach(NeuronGroup* tmpNeurGrp, neurGrpList){
		if(tmpNeurGrp->getInfo().getName().toUpper() == "INPUT LAYER"){
			inputNeuronGroup = tmpNeurGrp;
		}
		if(tmpNeurGrp->getInfo().getName().toUpper() == "FEATURE DETECTION"){
			featureNeuronGroup = tmpNeurGrp;
		}
		if(tmpNeurGrp->getInfo().getName().toUpper() == "OUTPUT 1"){
			if(tmpNeurGrp->size() != 1)
				throw SpikeStreamException("Output neuron group 1 is expected to have 1 neuron.");
			outputNeuron1ID = tmpNeurGrp->getNeuronIDs().at(0);
		}
		if(tmpNeurGrp->getInfo().getName().toUpper() == "OUTPUT 2"){
			if(tmpNeurGrp->size() != 1)
				throw SpikeStreamException("Output neuron group 2 is expected to have 1 neuron.");
			outputNeuron2ID = tmpNeurGrp->getNeuronIDs().at(0);
		}

		if(inputNeuronGroup != NULL && featureNeuronGroup != NULL)
			break;
	}
	if(inputNeuronGroup == NULL || featureNeuronGroup == NULL || outputNeuron1ID == 0 || outputNeuron2ID == 0)
		throw SpikeStreamException("Input and/or feature neuron group(s) not found.");

	numInputNeurons = inputNeuronGroup->size();
	inputXStart = inputNeuronGroup->getBoundingBox().getX1();
	inputYStart = inputNeuronGroup->getBoundingBox().getY1();
	inputZStart = inputNeuronGroup->getBoundingBox().getZ1();

	start();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Applys a sequence of firing patterns to the network */
void TemporalCodingExptManager::applyInputSequence(QList<Pattern>& patternList){
	for(int i=0; i<patternList.size() && !stopThread; ++i){
		patternList.at(i).print();
		nemoWrapper->setFiringInjectionPattern(patternList.at(i), inputNeuronGroup->getID(), false);
		stepNemo(1);
	}
}

/*! Returns a random sequence of numbers within the range of the number of time steps. */
QList<unsigned> TemporalCodingExptManager::getRandomSequence(){
	QList<unsigned> tmpNumList;
	for(int i=1; i<=numInputNeurons; ++i){
		tmpNumList.append(Util::getRandomUInt(0, numTimeSteps));
	}
	qDebug()<<"TMP NUM LIST RANDOM: "<<tmpNumList;
	return tmpNumList;
}


/*! Should be called after pattern injection. Looks for results
	in the feature detection layer and prints the resulting spikes. */
void TemporalCodingExptManager::getResults(){
	//Step and accumulate results
	QList<neurid_t> resultsList;
	QString resultsStr = "";
	timestep_t firstResultsTimeStep = 0;
	for(int i=0; i<numResultSteps && !stopThread; ++i){
		QList<neurid_t> tmpList = nemoWrapper->getFiringNeuronIDs();
		foreach(neurid_t tmpNeurID, tmpList){
			if(featureNeuronGroup->contains(tmpNeurID)){
				//Record time step of first results
				if(firstResultsTimeStep == 0)
					firstResultsTimeStep = nemoWrapper->getTimeStep();

				resultsList.append(tmpNeurID);
				resultsStr += QString::number(tmpNeurID) + ", ";
			}
		}

		//Take neurons during result integration period and ignore the rest
		if(resultsList.isEmpty() || !(nemoWrapper->getTimeStep() - firstResultsTimeStep >= numResultIntegrationSteps)){
			stepNemo(1);
		}
		//Have found all the feature detecting neurons that we want
		else{
			//Apply learning if required
			if(learning){
				if(experimentNumber == EXPERIMENT1){
					QList<neurid_t> tmpNeurIDList;
					tmpNeurIDList.append(outputNeuron1ID);
					nemoWrapper->setFiringNeuronIDs(tmpNeurIDList);
				}
				if(experimentNumber == EXPERIMENT2){
					QList<neurid_t> tmpNeurIDList;
					tmpNeurIDList.append(outputNeuron2ID);
					nemoWrapper->setFiringNeuronIDs(tmpNeurIDList);
				}
			}

			//Exit loop
			break;
		}
	}
	emit statusUpdate("Feature detector firing neurons: " + resultsStr);
}


/*! Returns a sequence of patterns encoding a set of numbers */
QList<Pattern> TemporalCodingExptManager::getTemporalPatternSequence(QList<unsigned>& numberList){
	//Create empty patterns. Need 1 pattern for each time step
	QList<Pattern> tmpPatternList;
	for(int i=0; i<numTimeSteps; ++i){
		//Add a couple of default points to ensure alignment
		Pattern tmpPattern;
		tmpPattern.addPoint(Point3D(inputXStart - 1.0f, inputYStart - 1.0f, inputZStart));
		tmpPattern.addPoint(Point3D(inputXStart + numInputNeurons, inputYStart + 1, inputZStart));
		tmpPatternList.append(tmpPattern);
	}

	//Add test data to patterns and return list
	for(int i=0; i<numInputNeurons; ++i){
		qDebug()<<"Adding point: x="<<(inputXStart + i)<<"; y="<<inputYStart<<"; z="<<inputZStart;
		tmpPatternList[numberList.at(i)].addPoint(Point3D(inputXStart + i, inputYStart, inputZStart));
	}
	return tmpPatternList;
}


/*! Returns a sequence of ascending numbers with noise */
QList<unsigned> TemporalCodingExptManager::getUpTrend(){
	QList<unsigned> tmpNumList;
	for(int i=1; i<=numInputNeurons; ++i){
		double tmpNum = ((double)(numTimeSteps) * ( (double)i/(double)(numInputNeurons))) - 1;
		if(tmpNum  < 0.0)
			tmpNum = 0.0;
		qDebug()<<"TEMP NUM: "<<tmpNum;
		tmpNumList.append(Util::rInt(tmpNum));
	}
	qDebug()<<"TMP NUM LIST: "<<tmpNumList;
	return tmpNumList;
}


/*! Returns a sequence of ascending numbers with noise */
QList<unsigned> TemporalCodingExptManager::getUpDownTrend(){
	QList<unsigned> tmpNumList;
	for(int i=0; i<numInputNeurons; i+=2){
		tmpNumList.append(Util::rInt( (double)(numTimeSteps-1) * ( (double)i/(double)(numInputNeurons) ) ) );
	}
	for(int i=numInputNeurons-1; i>=0; i-=2){
		tmpNumList.append(Util::rInt( (double)(numTimeSteps-1) * ( (double)i/(double)(numInputNeurons) ) ) );
	}
	return tmpNumList;
}


/*! Returns a sequence of descending numbers with noise */
QList<unsigned> TemporalCodingExptManager::getDownTrend(){
	QList<unsigned> tmpNumList;
	for(int i=numInputNeurons; i>0; --i){
		double tmpNum = ((double)(numTimeSteps) * ( (double)i/(double)(numInputNeurons))) - 1;
		if(tmpNum  < 0.0)
			tmpNum = 0.0;
		tmpNumList.append(Util::rInt(tmpNum));
	}
	return tmpNumList;
}


/*! Returns a sequence of descending numbers with noise */
QList<unsigned> TemporalCodingExptManager::getDownUpTrend(){
	QList<unsigned> tmpNumList;
	for(int i=numInputNeurons-1; i>=0; i-=2){
		tmpNumList.append(Util::rInt( (double)(numTimeSteps-1) * ( (double)i/(double)(numInputNeurons) ) ) );
	}
	for(int i=0; i<numInputNeurons; i+=2){
		tmpNumList.append(Util::rInt( (double)(numTimeSteps-1) * ( (double)i/(double)(numInputNeurons) ) ) );
	}
	return tmpNumList;
}


/*! Runs the experiment. */
void TemporalCodingExptManager::runExperiment1(){
	emit statusUpdate("Injecting up trend.");
	QList<unsigned> numberList = getUpTrend();
	QList<Pattern> patternList = getTemporalPatternSequence(numberList);
	applyInputSequence(patternList);
	getResults();
	stepNemo(numInterExptSteps);
}


/*! Runs the experiment. */
void TemporalCodingExptManager::runExperiment2(){
	emit statusUpdate("Injecting down trend.");
	QList<unsigned> numberList = getDownTrend();
	QList<Pattern> patternList = getTemporalPatternSequence(numberList);
	applyInputSequence(patternList);
	getResults();
	stepNemo(numInterExptSteps);
}


/*! Runs the experiment. */
void TemporalCodingExptManager::runExperiment3(){
	emit statusUpdate("Injecting down/up trend.");
	QList<unsigned> numberList = getDownUpTrend();
	QList<Pattern> patternList = getTemporalPatternSequence(numberList);
	applyInputSequence(patternList);
	getResults();
	stepNemo(numInterExptSteps);
}


/*! Runs the experiment. */
void TemporalCodingExptManager::runExperiment4(){
	emit statusUpdate("Injecting up/down trend.");
	QList<unsigned> numberList = getUpDownTrend();
	QList<Pattern> patternList = getTemporalPatternSequence(numberList);
	applyInputSequence(patternList);
	getResults();
	stepNemo(numInterExptSteps);
}


/*! Runs the experiment. */
void TemporalCodingExptManager::runExperiment5(){
	emit statusUpdate("Injecting random sequence.");
	QList<unsigned> numberList = getRandomSequence();
	QList<Pattern> patternList = getTemporalPatternSequence(numberList);
	applyInputSequence(patternList);
	getResults();
	stepNemo(numInterExptSteps);
}


/*! Advances the simulation by the specified number of time steps */
void TemporalCodingExptManager::stepNemo(unsigned numTimeSteps){
	for(unsigned i=0; i<numTimeSteps && !stopThread; ++i){
		nemoWrapper->stepSimulation();
		while((nemoWrapper->isWaitForGraphics() || nemoWrapper->getCurrentTask() == NemoWrapper::STEP_SIMULATION_TASK) && !stopThread)
			msleep(pauseInterval_ms);
	}
	msleep(pauseInterval_ms);
}


/*! Stores the parameters for the experiment */
void TemporalCodingExptManager::storeParameters(QHash<QString, double> &parameterMap){
	if(!parameterMap.contains("experiment_number"))
		throw SpikeStreamException("TemporalCodingExptManager: experiment_number parameter missing");
	experimentNumber = (int)parameterMap["experiment_number"];

	if(!parameterMap.contains("random_seed"))
		throw SpikeStreamException("TemporalCodingExptManager: random_seed parameter missing");
	randomSeed = (int)parameterMap["random_seed"];

	if(!parameterMap.contains("pause_interval_ms"))
		throw SpikeStreamException("TemporalCodingExptManager: pause_interval_ms parameter missing");
	pauseInterval_ms = (int)parameterMap["pause_interval_ms"];

	if(!parameterMap.contains("num_time_steps"))
		throw SpikeStreamException("TemporalCodingExptManager: num_time_steps parameter missing");
	numTimeSteps = (timestep_t)parameterMap["num_time_steps"];

	if(!parameterMap.contains("num_inter_expt_steps"))
		throw SpikeStreamException("TemporalCodingExptManager: num_inter_expt_steps parameter missing");
	numInterExptSteps = (timestep_t)parameterMap["num_inter_expt_steps"];

	if(!parameterMap.contains("num_result_steps"))
		throw SpikeStreamException("TemporalCodingExptManager: num_result_steps parameter missing");
	numResultSteps = (timestep_t)parameterMap["num_result_steps"];

	if(!parameterMap.contains("result_integration_steps"))
		throw SpikeStreamException("TemporalCodingExptManager: result_integration_steps parameter missing");
	numResultIntegrationSteps = (timestep_t)parameterMap["result_integration_steps"];

	if(!parameterMap.contains("learning"))
		throw SpikeStreamException("TemporalCodingExptManager: learning parameter missing");
	learning = (bool)parameterMap["learning"];
}




