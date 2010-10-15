#include "Globals.h"
#include "NeuronGroup.h"
#include "Topographic1BuilderThread.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Enables output of debugging information.
//#define DEBUG

/*! Constructor */
Topographic1BuilderThread::Topographic1BuilderThread() : AbstractConnectionBuilder() {
}


/*! Destructor */
Topographic1BuilderThread::~Topographic1BuilderThread(){
}


/*----------------------------------------------------------*/
/*-----                PROTECTED METHODS               -----*/
/*----------------------------------------------------------*/

/*! Returns a neuron group whose neurons are constructed according to the
	parameters in the neuron group info. */
void Topographic1BuilderThread::buildConnectionGroup(){
	//Sort out connection and neuron groups
	newConnectionGroup = new ConnectionGroup(connectionGroupInfo);
	NeuronGroup* fromNeurGrp = Globals::getNetwork()->getNeuronGroup(connectionGroupInfo.getFromNeuronGroupID());
	NeuronGroup* toNeurGrp = Globals::getNetwork()->getNeuronGroup(connectionGroupInfo.getToNeuronGroupID());

	//Extract parameters
	projWidth = getParameter("projection_width");
	projLength = getParameter("projection_length");
	projHeight = getParameter("projection_height");
	ovWidth = getParameter("overlap_width");
	ovLength = getParameter("overlap_length");
	ovHeight = getParameter("overlap_height");
	projectionPosition = (int)getParameter("projection_position");
	forwardReverse = (bool)getParameter("forward_reverse");
	conPattern = getParameter("connection_pattern");
	minWeight = getParameter("min_weight");
	maxWeight = getParameter("max_weight");
	delayType = getParameter("delay_type");
	delayDistanceFactor = 0;
	minDelay = 0, maxDelay = 0;
	if(delayType == 0){
		delayDistanceFactor = getParameter("delay_distance_factor");
	}
	else{
		minDelay = (unsigned)getParameter("min_delay");
		maxDelay = (unsigned)getParameter("max_delay");
	}
	density = getParameter("density");

	//Keep track of progress
	numberOfProgressSteps = fromNeurGrp->size() + 1;

	//Work through all of the from neurons to build the projection space
	QHash<unsigned, Box> projBoxMap;
	Point3D tmpPnt;
	int xCount=0, yCount=0, zCount=0;
	float tmpX, tmpY, tmpZ, prevX, prevY, prevZ, boxXPos, boxYPos, boxZPos;
	bool firstTime = true;
	for(NeuronPositionIterator posIter = fromNeurGrp->positionBegin(); posIter != fromNeurGrp->positionEnd(); ++posIter){
		tmpPnt = posIter.value()->getLocation();
		tmpX = tmpPnt.getXPos();
		tmpY = tmpPnt.getYPos();
		tmpZ = tmpPnt.getZPos();

		//Initialize previous values on first call
		if(firstTime){
			prevX = tmpX;
			prevY = tmpY;
			prevZ = tmpZ;
			firstTime = false;
		}

		//Sanity check
		if(tmpX < prevX)
			throw SpikeStreamException("Count along X axis should always increase. tmpX=" + QString::number(tmpX));

		//Increase counts of neurons along x, y and z axes
		if(tmpX > prevX){
			++xCount;
			yCount = 0;
			zCount = 0;
		}
		else if(tmpY > prevY){
			++yCount;
			zCount =0;
		}
		else if(tmpZ > prevZ){
			++zCount;
		}

		//Create new box
		boxXPos = xCount * (projWidth - ovWidth);
		boxYPos = yCount * (projLength - ovLength);
		boxZPos = zCount * (projHeight - ovHeight);
		Box newBox(boxXPos, boxYPos, boxZPos, boxXPos + projWidth, boxYPos + projLength, boxZPos + projHeight);
		if(projBoxMap.contains(posIter.value()->getID()))
			throw SpikeStreamException("Duplicate neuron ID: " + QString::number(posIter.value()->getID()));
		projBoxMap[posIter.value()->getID()] = newBox;

		//Store location for next iteration
		prevX = tmpX;
		prevY = tmpY;
		prevZ = tmpZ;
	}

	//Get a single box enclosing all of the boxes that have been created
	Box totalBox = Box::getEnclosingBox(projBoxMap.values());

	/* Now have a projection volume with its origin at (0,0,0)
		Need to translate this volume so that its centre is aligned with the centre of the to neuron group. */
	Point3D fromCentre = totalBox.centre();
	Point3D toCentre = toNeurGrp->getBoundingBox().centre();
	float dx = toCentre.getXPos() - fromCentre.getXPos();
	float dy = toCentre.getYPos() - fromCentre.getYPos();
	float dz = toCentre.getZPos() - fromCentre.getZPos();

	//Translate boxes and create connections
	int neuronCntr = 0;
	NeuronIterator fromNeurGrpEnd = fromNeurGrp->end();
	for(NeuronIterator iter = fromNeurGrp->begin(); iter != fromNeurGrpEnd; ++iter){
		if(!projBoxMap.contains(iter.key()))
			throw SpikeStreamException("Neuron ID missing from projection box map: " + QString::number(iter.key()));
		projBoxMap[iter.key()].translate(dx, dy, dz);
		addProjectiveConnections(iter.value(), toNeurGrp, projBoxMap[iter.key()]);

		//Inform user about progress
		emit progress(neuronCntr, numberOfProgressSteps, "Building connections...");
		++neuronCntr;
	}
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds projective connections from the neuron to all neurons in the box
	 using parameters supplied. */
void Topographic1BuilderThread::addProjectiveConnections(Neuron* fromNeuron, NeuronGroup* toNeurGrp, Box& projBox){
	float radius = 0.5 * Util::min(projBox.getWidth(), projBox.getLength(), projBox.getHeight());
	Point3D projBoxCentre = projBox.centre();

	QList<Neuron*> neurList = toNeurGrp->getNeurons(projBox);
	Neuron* tmpNeur;
	for(int i=0; i<neurList.size(); ++i){
		tmpNeur = neurList.at(i);
		if(conPattern == GAUSSIAN_SPHERE){
			if(makeGaussianConnection(projBox, tmpNeur->getLocation())){
				newConnectionGroup->addConnection(fromNeuron->getID(), tmpNeur->getID(), getDelay(fromNeuron, tmpNeur), getWeight());
			}
		}
		else if(conPattern == UNIFORM_SPHERE){
			double ranNum = (double)rand() / (double)RAND_MAX;
			if(ranNum <= density){//Decide if connection is made
				if(tmpNeur->getLocation().distance(projBoxCentre) < radius){
					newConnectionGroup->addConnection(fromNeuron->getID(), tmpNeur->getID(), getDelay(fromNeuron, tmpNeur), getWeight());
				}
			}
		}
		else if(conPattern == UNIFORM_CUBE){
			double ranNum = (double)rand() / (double)RAND_MAX;
			if(ranNum <= density){//Decide if connection is made
				newConnectionGroup->addConnection(fromNeuron->getID(), tmpNeur->getID(), getDelay(fromNeuron, tmpNeur), getWeight());
			}
		}
		else{
			throw SpikeStreamException("Connection pattern not recognized: " + conPattern);
		}

	}
}


/*! Extracts parameters from neuron group info and checks that they are in range. */
void Topographic1BuilderThread::checkParameters(){
	double projectionWidth = getParameter("projection_width");
	double projectionLength = getParameter("projection_length");
	double projectionHeight = getParameter("projection_height");
	double overlapWidth = getParameter("overlap_width");
	double overlapLength = getParameter("overlap_length");
	double overlapHeight = getParameter("overlap_height");
	int projectionPosition = (int)getParameter("projection_position");
	getParameter("forward_reverse");
	int connectionPattern = getParameter("connection_pattern");
	double minWeight = getParameter("min_weight");
	double maxWeight = getParameter("max_weight");
	int delayType = getParameter("delay_type");
	double density = getParameter("density");

	if(projectionWidth <= 0.0)
		throw SpikeStreamException("Projection width must be greater than 0: " + QString::number(projectionWidth));
	if(projectionLength <= 0.0)
		throw SpikeStreamException("Projection length must be greater than 0: " + QString::number(projectionLength));
	if(projectionHeight <= 0.0)
		throw SpikeStreamException("Projection height must be greater than 0: " + QString::number(projectionHeight));

	if(overlapWidth < 0.0)
		throw SpikeStreamException("Overlap width cannot be less than 0: " + QString::number(overlapWidth));
	if(overlapLength < 0.0)
		throw SpikeStreamException("Overlap length cannot be less than 0: " + QString::number(overlapLength));
	if(overlapHeight < 0.0)
		throw SpikeStreamException("Overlap height cannot be less than 0: " + QString::number(overlapHeight));

	if(projectionPosition < 0 || projectionPosition > 4)
		throw SpikeStreamException("Projection position not recognized: " + QString::number(projectionPosition));

	if(!(connectionPattern == GAUSSIAN_SPHERE || connectionPattern == UNIFORM_SPHERE || connectionPattern == UNIFORM_CUBE))
		throw SpikeStreamException("Connection pattern not recognized: " + QString::number(connectionPattern));

	if(minWeight > maxWeight)
		throw SpikeStreamException("Min weight cannot be greater than max weight.");

	if(!(delayType == DELAY_WITH_DISTANCE || delayType == RANDOM_DELAY))
		throw SpikeStreamException("Delay type not recognized: " + QString::number(delayType));

	if(delayType == DELAY_WITH_DISTANCE){
		double delayDistanceFactor = getParameter("delay_distance_factor");
		if(delayDistanceFactor <= 0.0)
			throw SpikeStreamException("Delay distance factor cannot be less than or equal to 0: " + QString::number(delayDistanceFactor));
	}
	else if (delayType == RANDOM_DELAY){
		unsigned minDelay = (unsigned)getParameter("min_delay");
		unsigned maxDelay = (unsigned)getParameter("max_delay");
		if(minDelay > maxDelay)
			throw SpikeStreamException("Min delay cannot be greater than max delay.");
	}

	if(density <= 0.0)
		throw SpikeStreamException("Density must be greater than 0: " + QString::number(density));
}


/*! Returns delay that will vary with the distance of the two neurons or randomly selected from a range */
float Topographic1BuilderThread::getDelay(Neuron *fromNeuron, Neuron *toNeuron){
	if(delayType == DELAY_WITH_DISTANCE){
		return delayDistanceFactor * fromNeuron->getLocation().distance(toNeuron->getLocation());
	}
	else if (delayType == RANDOM_DELAY){
		return Util::getRandomUInt(minDelay, maxDelay);
	}
	else
		throw SpikeStreamException("Delay type not recognized: " + QString::number(delayType));
}


/*! Returns weight that is randomly selected from the specified range */
float Topographic1BuilderThread::getWeight(){
	return Util::getRandomDouble(minWeight, maxWeight);
}


/*! Returns normally distributed true and false depending on the relationshiop between
	the point and the box. The closer the point is to the centre of the box the more likely
	that the respose will be true. */
bool Topographic1BuilderThread::makeGaussianConnection(const Box& projectionBox, const Point3D neuronLocation){
	//Get the connection probability on each axis
	float radius = projectionBox.getWidth()/2.0;
	float distance = Util::toPositive(neuronLocation.getXPos() - (projectionBox.getX1() + radius) );
	if(!makeGaussianConnection(radius, distance))
		return false;

	radius = projectionBox.getLength()/2.0;
	distance = Util::toPositive(neuronLocation.getYPos() - (projectionBox.getY1() + radius) );
	if(!makeGaussianConnection(radius, distance))
		return false;

	radius = projectionBox.getHeight()/2.0;
	distance = Util::toPositive(neuronLocation.getZPos() - (projectionBox.getZ1() + radius) );
	if(!makeGaussianConnection(radius, distance))
		return false;

	return true;
}


/*! Returns normally distributed true and false depending on the relationshiop between
	the distance and the radius. The closer the distance is to zero, the more likely
	that the respose will be true. */
bool Topographic1BuilderThread::makeGaussianConnection(float radius, float distance){
	//Get normally distributed random number
	double normRan = Util::toPositive(getNormalRandom());

	//Limit it to + 3
	if(normRan > 3.0)
		normRan = 3.0;

	//Normal random number now varies between -3 and +3 so need to make it fit into the radius range
	normRan *= (radius / 3.0);

	/* 68% of values will be between 0 and 1. Adding 1 makes little difference to large networks and
		creates a better connection pattern on small networks. */
	++normRan;

	#ifdef DEBUG
		qDebug()<<"NormRan="<<normRan<<"; Radius="<<radius<<"; distance="<<distance<<"; density factor="<<(1.0f/density);
	#endif//DEBUG

	/* Should now have a number that is between 1 and radius and
		much more likely to be closer to 0 than to radius
		Accept connection if normRan is greater than the distance.
		The shorter the distance, the more likely this is to occur.
		Higher values of density will reduce the distance and make the connection more likely. */
	if(normRan >= (1.0f/density)* distance)
		return true;
	return false;
}


/*! Returns a normally distributed random number with standar deviation = 1
	Uses Box-Muller method to generate values
	Code adapted from http://www.csit.fsu.edu/~burkardt/cpp_src/random_data/random_data.html. */
double Topographic1BuilderThread::getNormalRandom(){
	double PI = 3.141592653589793;
	double rand1, rand2;
	static int used = 0;
	double x;
	static double y = 0.0;
	//  If we've used an even number of values so far, generate two more, return one and save one.
	if (( used % 2 ) == 0 ){
		for ( ; ; ){
			rand1 = (double)rand()/(double)RAND_MAX;
			if ( rand1 != 0.0 )
				break;
		}
		rand2 = (double)rand()/(double)RAND_MAX;
		x = sqrt ( -2.0 * log ( rand1 ) ) * cos ( 2.0 * PI * rand2 );
		y = sqrt ( -2.0 * log ( rand1 ) ) * sin ( 2.0 * PI * rand2 );
	}
	//  Otherwise, return the second, saved, value.
	else{
		x = y;
	}
	++used;
	return x;
}
