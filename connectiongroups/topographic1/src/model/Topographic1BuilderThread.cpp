#include "Globals.h"
#include "NeuronGroup.h"
#include "Topographic1BuilderThread.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

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

//	numberOfProgressSteps = fromNeurGrp->size();

	//Get the neuron in the FROM group that projects to the centre of the TO group
	Neuron* centreNeuron = fromNeurGrp->getNearestNeuron(fromNeurGrp->getBoundingBox().centre());

	//Get the centre of the TO neuron group
	Point3D toCent = toNeurGrp->getBoundingBox().centre();
	Box projBox(
			toCent.getXPos() - projWidth/2.0f, toCent.getYPos() - projLength/2.0f, toCent.getZPos() - projHeight/2.0f,
			toCent.getXPos() + projWidth/2.0f, toCent.getYPos() + projLength/2.0f, toCent.getZPos() + projHeight/2.0f
	);

	//Add connections to group
	addProjectiveConnections(centreNeuron, toNeurGrp, projBox);


	emit progress(1, 2, "Building connections...");



	//		//Update progress
	//		++cntr;
	//		emit progress(cntr, numberOfProgressSteps, "Building connections...");
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds projective connections from the neuron to all neurons in the box
	 using parameters supplied. */
void Topographic1BuilderThread::addProjectiveConnections(Neuron* centreNeuron, NeuronGroup* toNeurGrp, Box& projBox){
	QList<Neuron*> neurList = toNeurGrp->getNeurons(projBox);
	for(int i=0; i<neurList.size(); ++i){
		if(conPattern == GAUSSIAN_SPHERE){

		}
		else if(conPattern == UNIFORM_SPHERE){

		}
		else if(conPattern == UNIFORM_CUBE){
			newConnectionGroup->addConnection( new Connection(centreNeuron->getID(), neurList.at(i), getDelay(), getWeight() ) );
		}
		else
			throw SpikeStreamException("Connection pattern not recognized: " + conPattern);
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

	if(!(connectionPattern == GAUSSIAN_SPHERE || connectionPattern == UNIFORM_SPHERE || connectionPattern != UNIFORM_CUBE))
		throw SpikeStreamException("Connection pattern not recognized: " + QString::number(connectionPattern));

	if(minWeight > maxWeight)
		throw SpikeStreamException("Min weight cannot be greater than max weight.");

	if(delayType < 0 || delayType > 1)
		throw SpikeStreamException("Delay type not recognized: " + QString::number(delayType));

	if(delayType == 0){
		double delayDistanceFactor = getParameter("delay_distance_factor");
		if(delayDistanceFactor <= 0.0)
			throw SpikeStreamException("Delay distance factor cannot be less than or equal to 0: " + QString::number(delayDistanceFactor));
	}
	else if (delayType ==1){
		unsigned minDelay = (unsigned)getParameter("min_delay");
		unsigned maxDelay = (unsigned)getParameter("max_delay");
		if(minDelay > maxDelay)
			throw SpikeStreamException("Min delay cannot be greater than max delay.");
	}

	if(density < 0.0 || density > 1.0)
		throw SpikeStreamException("Density must be between 0.0 and 1.0: " + QString::number(density));
}


