//SpikeStream includes
#include "Pop1ExperimentManager.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
Pop1ExperimentManager::Pop1ExperimentManager() : SpikeStreamThread(){
}


/*! Destructor */
Pop1ExperimentManager::~Pop1ExperimentManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void Pop1ExperimentManager::run(){
	clearError();
	stopThread = false;
	unsigned origWaitInterval = nemoWrapper->getWaitInterval_ms();
	nemoWrapper->setWaitInterval(1);//Minimal wait between steps

	try{
		getAverageHammingDistanceNoise();
		runExperiment();
	}
	catch(SpikeStreamException& ex){
		setError(ex.getMessage());
	}
	catch(...){
		setError("Pop1ExperimentManager has thrown an unknown exception.");
	}

	nemoWrapper->setWaitInterval(origWaitInterval);//Restore wrapper to original state
	experimentNumber = NO_EXPERIMENT;
	stopThread = true;
}


/*! Sets up the experiment and starts thread running. */
void Pop1ExperimentManager::startExperiment(NemoWrapper* nemoWrapper, NeuronGroup* neuronGroup, QHash<QString, double>& parameterMap){
	this->neuronGroup = neuronGroup;
	this->nemoWrapper = nemoWrapper;
	storeParameters(parameterMap);
	start();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds noise to injection pattern.
	This jumps the specified number of columns by the noise amount. */
void Pop1ExperimentManager::addHammingNoise(QList<Pattern>& patternList){
	QList<Pattern> newPatternList;

	for(int i=0; i<patternList.size(); ++i){
		//Get the points in the current pattern
		QList<Point3D> pointList = patternList.at(i).getPoints();

		//Get a set of columns that will be changed. Ignore the last two points, which are there for alignment
		QList<int> selectionIndexes = getRandomSelectionIndexes(numNoiseCols, pointList.size()-2);

		//Change the points at the selection indexes
		for(int j=0; j<selectionIndexes.size(); ++j){
			//Get the original Y position
			float origY = pointList[selectionIndexes.at(j)].getYPos();

			//Select shift direction at random - shift up if possible, otherwise shift down
			if(Util::getRandom(0,2) == 0){
				if(origY + randomShiftAmount < yStart + numYNeurons)
					pointList[selectionIndexes.at(j)].setYPos(origY + randomShiftAmount);
				else
					pointList[selectionIndexes.at(j)].setYPos(origY - randomShiftAmount);
			}

			//Shift down if possible, otherwise shift up
			else{
				if(origY - randomShiftAmount >= yStart)
					pointList[selectionIndexes.at(j)].setYPos(origY - randomShiftAmount);
				else
					pointList[selectionIndexes.at(j)].setYPos(origY + randomShiftAmount);
			}
		}

		//Create new pattern with the modified points
		Pattern tmpPattern;
		for(int j=0; j<pointList.size(); ++j)
			tmpPattern.addPoint(pointList.at(j));
		newPatternList.append(tmpPattern);
		qDebug()<<"=============== ORIGINAL PATTERN =====================";
		patternList.at(i).print();
		qDebug()<<"=============== PERTURBED PATTERN =====================";
		tmpPattern.print();
	}

	//Copy new patterns into original pattern list
	patternList = newPatternList;
}


/*! Calculates the average Hamming distance that would be expected per pattern
	if the signals were completely uncorrelated */
int Pop1ExperimentManager::getAverageHammingDistanceNoise(){
	QList<int> resultsList;

	//Work through different seeds starting with the initial seed
	for(int seedCtr = 0; seedCtr < 10; ++seedCtr){
		Util::seedRandom(randomSeed + seedCtr*50);

		int hammingTot = 0;
		for(int i=0; i<numPatterns; ++i){
			for(int j=0; j<numXNeurons; ++j){
				hammingTot += Util::toPositive(Util::getRandom(yStart, numYNeurons+yStart) - Util::getRandom(yStart, numYNeurons+yStart));
			}
		}
		resultsList.append(hammingTot);
	}

	//Calculate the averate
	double average = 0.0;
	for(int i=0; i<resultsList.size(); ++i)
		average += resultsList.at(i);
	average /= resultsList.size();

	//Calculate the sample standard deviation
	double sd = 0.0;
	for(int i=0; i<resultsList.size(); ++i)
		sd += pow(average - resultsList.at(i), 2);
	sd /= (resultsList.size() -1 );
	sd = sqrt(sd);

	emit statusUpdate("Average total Hamming distance between uncorrelated patterns: " + QString::number(average) + "; population standard deviation: " + QString::number(sd));
	return average;
}


/*! Returns the Hamming distance between two firing neuron patterns.
	Each jump along the Y axis adds 1 to the Hamming distance. There is no
	wrap around because the firing patterns are representing real numbers.
	If neurons are missing, Hamming distance is increased by 10. */
int Pop1ExperimentManager::getHammingDistance(const QList<neurid_t>& expectedList, QList<neurid_t> actualList){
	//Convert expected and actual values to 2 arrays with x running from 0-9 along the bottom
	int expectedArray[numXNeurons];
	int actualArray[numXNeurons];
	for(int i=0; i<numXNeurons; ++i){
		expectedArray[i] = 0;
		actualArray[i] = 0;
	}

	//Extract expected values
	int tmpXPos, tmpYPos;
	foreach(neurid_t expectedID, expectedList){
		tmpXPos = getXValue(expectedID);
		tmpYPos = getYValue(expectedID);
		//qDebug()<<"Expected list size: "<<expectedList.size()<<"; expected ID="<<expectedID<<"; start neuron id="<<neuronGroup->getStartNeuronID()<<"; tmpXPos="<<tmpXPos<<"; tmpYPos="<<tmpYPos;
		if(tmpXPos < 0 || tmpYPos < 0 || tmpXPos >= numXNeurons || tmpYPos >= numYNeurons)
			throw SpikeStreamException("Expected X or Y position out of range: " + QString::number(tmpXPos) + ", " + QString::number(tmpYPos));

		//Handle multiple firing neurons
		if(expectedArray[tmpXPos] != 0)
			throw SpikeStreamException("Multiple firing neurons in expected pattern along Y axis");
		expectedArray[tmpXPos] = tmpYPos;
	}

	//Extract actual values
	foreach(neurid_t actualID, actualList){
		tmpXPos = getXValue(actualID);
		tmpYPos = getYValue(actualID);
		if(tmpXPos < 0 || tmpYPos < 0 || tmpXPos >= numXNeurons || tmpYPos >= numYNeurons)
			throw SpikeStreamException("Actual X or Y position out of range: " + QString::number(tmpXPos) + ", " + QString::number(tmpYPos));

		//Handle multiple firing neurons along Y direction - take average
		if(actualArray[tmpXPos] != 0)
			actualArray[tmpXPos] = Util::rInt( (tmpYPos + actualArray[tmpXPos]) / 2.0 );
		else
			actualArray[tmpXPos] = tmpYPos;
	}

	//Get Hamming distance between the expected and actual arrays
	unsigned hammingDist = 0;
	for(int i=0; i<numXNeurons; ++i){
		hammingDist += Util::toPositive(expectedArray[i] - actualArray[i]);
		//qDebug()<<"Expected array["<<i<<"]="<<expectedArray[i]<<"; actual array["<<i<<"]="<<actualArray[i]<<"; Hamming dist = "<<hammingDist;
	}
	return hammingDist;
}


/*! Returns a random pattern */
Pattern Pop1ExperimentManager::getPattern(){
	Pattern tmpPattern;
	float xPos, yPos, zPos = zStart;
	for(int i=1; i<=numXNeurons; ++i){
		xPos = i;
		yPos = Util::getRandom(yStart, yStart + numYNeurons);
		tmpPattern.addPoint(Point3D(xPos, yPos, zPos));
	}

	//Add a couple of points to ensure alignment
	tmpPattern.addPoint(Point3D(xStart - 1.0f, yStart-1.0f, zStart));
	tmpPattern.addPoint(Point3D(xStart + numXNeurons, yStart + numYNeurons, zStart));

	return tmpPattern;
}


/*! Generates the specified number of non repeating random indexes
	starting 0 and finishing at max-1 */
QList<int> Pop1ExperimentManager::getRandomSelectionIndexes(int numSelections, int max){
	if(numSelections > max)
		throw SpikeStreamException("Number of selections must be less or equal to the maximum. Selections start at 0 and end at max-1.");

	QHash<int, bool> selectionMap;
	while(selectionMap.size() != numSelections){
		selectionMap[Util::getRandom(0, max)] = true;
	}

	return selectionMap.keys();
}


/*! Returns the X value of a particular neurons location */
int Pop1ExperimentManager::getXValue(neurid_t neuronID){
	if(!neuronGroup->contains(neuronID))
		throw SpikeStreamException("Neuron ID cannot be found in neuron group: " + QString::number(neuronID));
	return (int)neuronGroup->getNeuronLocation(neuronID).getXPos() - xStart;//Neuron X positions start at 1
}


/*! Returns the Y value of a particular neuron's location */
int Pop1ExperimentManager::getYValue(neurid_t neuronID){
	if(!neuronGroup->contains(neuronID))
		throw SpikeStreamException("Neuron ID cannot be found in neuron group: " + QString::number(neuronID));
	return (int)neuronGroup->getNeuronLocation(neuronID).getYPos() - yStart;//Neuron Y positions start at 1
}


/*! Runs the experiment. */
void Pop1ExperimentManager::runExperiment(){
	//Variables used in the experiment
	QList<Pattern> patternList;
	QList< QList<neurid_t> > patternNeurIDList;

	//Seed the random number generator
	Util::seedRandom(randomSeed);

	//Train network on numPatterns patterns
	emit statusUpdate("Training network for Experiment " + QString::number(experimentNumber + 1));
	for(int i=0; i<numPatterns && !stopThread; ++i){
		//Create a random pattern
		Pattern tmpPattern = getPattern();
		patternList.append(tmpPattern);

		//Inject it into the simulation and step simulation
		emit statusUpdate("Injecting pattern " + QString::number(i));
		nemoWrapper->setFiringInjectionPattern(tmpPattern, neuronGroup->getID(), false);
		stepNemo(1);

		//Store the neuron IDs associated with the pattern
		patternNeurIDList.append(nemoWrapper->getFiringNeuronIDs());

		//Step another nine time steps before injecting the next pattern
		stepNemo(9);
	}

	//Step the network to clear any residual activity
	emit statusUpdate("Clearing residual network activity");
	stepNemo(20);

	//Add noise to injection pattern for experiment 2
	if(experimentNumber == EXPERIMENT2)
		addHammingNoise(patternList);
	else if(experimentNumber == EXPERIMENT3)
		subtractRandomPoints(patternList);

	//Inject each pattern and evaluate whether it has correctly invoked the subsequent pattern
	//The last pattern is not injected because it has not been trained on anything
	emit statusUpdate("Testing network");
	int totalHammingDist = 0;
	for(int i=0; i< numPatterns-1 && !stopThread; ++i){
		//Inject it into the simulation
		emit statusUpdate("Injecting pattern " + QString::number(i));
		nemoWrapper->setFiringInjectionPattern(patternList.at(i), neuronGroup->getID(), false);

		//Step simulation to point at which we expect the next pattern to appear
		stepNemo(stepsToPatternRead);

		//Output the Hamming distance between the actual firing pattern and the firing pattern we expect
		int hammingDist = getHammingDistance(patternNeurIDList.at(i+1), nemoWrapper->getFiringNeuronIDs());
		totalHammingDist += hammingDist;
		emit statusUpdate("Patten " + QString::number(i) + " produced a second pattern with a Hamming distance of " + QString::number(hammingDist) + "; " + QString::number(nemoWrapper->getFiringNeuronIDs().size()) + " firing neurons in second pattern.");

		//Step beyond the end of the association chain before injecting the next pattern
		emit statusUpdate("Waiting for sequence to finish.");
		stepNemo((numPatterns-i) * 15);
	}

	//Output final result
	emit statusUpdate("Total Hamming distance: " + QString::number(totalHammingDist));
}


/*! Advances the simulation by the specified number of time steps */
void Pop1ExperimentManager::stepNemo(unsigned numTimeSteps){
	for(unsigned i=0; i<numTimeSteps && !stopThread; ++i){
		nemoWrapper->stepSimulation();
		while((nemoWrapper->isWaitForGraphics() || nemoWrapper->getCurrentTask() == NemoWrapper::STEP_SIMULATION_TASK) && !stopThread)
			msleep(pauseInterval_ms);
	}
	msleep(pauseInterval_ms);
}


/*! Stores the parameters for the experiment */
void Pop1ExperimentManager::storeParameters(QHash<QString, double> &parameterMap){
	if(!parameterMap.contains("experiment_number"))
		throw SpikeStreamException("Pop1ExperimentManager: experiment_number parameter missing");
	experimentNumber = (int)parameterMap["experiment_number"];

	if(!parameterMap.contains("random_seed"))
		throw SpikeStreamException("Pop1ExperimentManager: random_seed parameter missing");
	randomSeed = (int)parameterMap["random_seed"];

	if(!parameterMap.contains("number_of_patterns"))
		throw SpikeStreamException("Pop1ExperimentManager: number_of_patterns parameter missing");
	numPatterns = (int)parameterMap["number_of_patterns"];

	if(!parameterMap.contains("number_of_x_neurons"))
		throw SpikeStreamException("Pop1ExperimentManager: number_of_x_neurons parameter missing");
	numXNeurons = (int)parameterMap["number_of_x_neurons"];

	if(!parameterMap.contains("number_of_y_neurons"))
		throw SpikeStreamException("Pop1ExperimentManager: number_of_y_neurons parameter missing");
	numYNeurons = (int)parameterMap["number_of_y_neurons"];

	if(!parameterMap.contains("x_start"))
		throw SpikeStreamException("Pop1ExperimentManager: x_start parameter missing");
	xStart = (float)parameterMap["x_start"];

	if(!parameterMap.contains("y_start"))
		throw SpikeStreamException("Pop1ExperimentManager: y_start parameter missing");
	yStart = (float)parameterMap["y_start"];

	if(!parameterMap.contains("z_start"))
		throw SpikeStreamException("Pop1ExperimentManager: z_start parameter missing");
	zStart = (float)parameterMap["z_start"];

	if(!parameterMap.contains("num_noise_cols"))
		throw SpikeStreamException("Pop1ExperimentManager: num_noise_cols parameter missing");
	numNoiseCols = (float)parameterMap["num_noise_cols"];

	if(!parameterMap.contains("random_shift_amount"))
		throw SpikeStreamException("Pop1ExperimentManager: random_shift_amount parameter missing");
	randomShiftAmount = (float)parameterMap["random_shift_amount"];

	if(!parameterMap.contains("steps_to_pattern_read"))
		throw SpikeStreamException("Pop1ExperimentManager: steps_to_pattern_read parameter missing");
	stepsToPatternRead = (int)parameterMap["steps_to_pattern_read"];

	if(!parameterMap.contains("pause_interval_ms"))
		throw SpikeStreamException("Pop1ExperimentManager: pause_interval_ms parameter missing");
	pauseInterval_ms = (int)parameterMap["pause_interval_ms"];
}


/*! Subtracts random points from the injection pattern used for testing. */
void Pop1ExperimentManager::subtractRandomPoints(QList<Pattern>& patternList){
	QList<Pattern> newPatternList;

	for(int i=0; i<patternList.size(); ++i){
		//Get the points in the current pattern
		QList<Point3D> pointList = patternList.at(i).getPoints();

		//Get a set of columns that will NOT their points removed. Ignore the last two points, which are there for alignment
		QList<int> selectionIndexes = getRandomSelectionIndexes(numXNeurons - numNoiseCols, numXNeurons);

		//Add selected points to the new pattern
		Pattern tmpPattern;
		for(int j=0; j<selectionIndexes.size(); ++j){
			tmpPattern.addPoint(pointList[selectionIndexes.at(j)]);
		}

		//Add last two points to the pattern
		tmpPattern.addPoint(pointList.at(pointList.size() - 1));
		tmpPattern.addPoint(pointList.at(pointList.size() - 2));

		//Create new pattern with the modified points
		newPatternList.append(tmpPattern);
		qDebug()<<"=============== ORIGINAL PATTERN =====================";
		patternList.at(i).print();
		qDebug()<<"=============== PERTURBED PATTERN =====================";
		tmpPattern.print();
	}

	//Copy new patterns into original pattern list
	patternList = newPatternList;
}



