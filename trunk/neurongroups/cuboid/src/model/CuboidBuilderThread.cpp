#include "CuboidBuilderThread.h"
#include "Globals.h"
#include "SpikeStreamException.h"
using namespace spikestream;

/*! Constructor */
CuboidBuilderThread::CuboidBuilderThread(){
}


/*! Destructor */
CuboidBuilderThread::~CuboidBuilderThread(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns the id of the newly created neuron group or 0 if no neuron group has been created */
unsigned int CuboidBuilderThread::getNeuronGroupID(){
	if(newNeuronGroup == NULL)
		return 0;
	return newNeuronGroup->getID();
}


/*! Prepares class before it runs as a separate thread to add a neuron group */
void CuboidBuilderThread::prepareAddNeuronGroup(const NeuronGroupInfo& neurGrpInfo){
	//Store information about the neuron group to be added
	this->neuronGroupInfo = neurGrpInfo;

	//Extract parameters from neuron group
	storeParameters();

	if(!Globals::networkLoaded())
		throw SpikeStreamException("Cannot add neuron group - no network loaded.");
}


/*! Thread run method */
void CuboidBuilderThread::run(){
	clearError();
	stopThread = false;
	newNeuronGroup = NULL;
	try{
		//Seed the random number generator
		srand(12345678);

		//Add the neuron group
		addNeuronGroup();
	}
	catch (SpikeStreamException& ex){
		setError(ex.getMessage());
	}
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds a neuron group with the specified parameters to the database */
void CuboidBuilderThread::addNeuronGroup(){
	Network* currentNetwork = Globals::getNetwork();

	//Need to set a new network and archive dao in the network because we are running as a separate thread
	NetworkDao newNetworkDao(Globals::getNetworkDao()->getDBInfo());
	ArchiveDao newArchiveDao(Globals::getArchiveDao()->getDBInfo());
	currentNetwork->setNetworkDao(&newNetworkDao);
	currentNetwork->setArchiveDao(&newArchiveDao);

	//Add neuron group to network
	buildNeuronGroup();
	QList<NeuronGroup*> neurGrpList;
	neurGrpList.append(newNeuronGroup);
	currentNetwork->addNeuronGroups(neurGrpList);

	//Wait for network to finish
	while(currentNetwork->isBusy())
		this->msleep(500);

	emit progress(width+2, width+2);
}


/*! Clears error state and message */
void CuboidBuilderThread::clearError(){
	error = false;
	errorMessage = "";
}


/*! Returns a neuron group whose neurons are constructed according to the
	parameters in the neuron group info. */
void CuboidBuilderThread::buildNeuronGroup(){
	newNeuronGroup = new NeuronGroup(neuronGroupInfo);

	//Add the neurons to the neuron group
	for(int xPos = xStart; xPos <= xStart+width && !stopThread; xPos += spacing){
		for(int yPos = yStart; yPos <= yStart+length && !stopThread; yPos += spacing){
			for(int zPos = zStart; zPos <= zStart+height && !stopThread; zPos += spacing){
				double ranNum = (double)rand() / (double)RAND_MAX;
				if(ranNum <= density)
					newNeuronGroup->addNeuron(xPos, yPos, zPos);
			}
		}

		//Give some idea about progress
		emit progress(xPos, width+2);
	}
}


/*! Returns a parameter from the neuron group info parameter map checking that it actually exists */
double CuboidBuilderThread::getParameter(const QString& paramName){
	QHash<QString, double> paramMap = neuronGroupInfo.getParameterMap();
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter with " + paramName + " does not exist in parameter map.");
	return paramMap[paramName];
}


void CuboidBuilderThread::setError(const QString& errorMessage){
	error = true;
	this->errorMessage = errorMessage;
	stopThread = true;
}


/*! Extracts parameters from neuron group info and stores them in the appropriate format */
void CuboidBuilderThread::storeParameters(){
	xStart = (int)getParameter("x");
	yStart = (int)getParameter("y");
	zStart = (int)getParameter("z");
	width = (int)getParameter("width");
	if(width <=0)
		throw SpikeStreamException("Width of neuron group is out of range: " + QString::number(width));
	length = (int)getParameter("length");
	if(length <=0)
		throw SpikeStreamException("Length of neuron group is out of range: " + QString::number(length));
	height = (int)getParameter("height");
	if(height <=0)
		throw SpikeStreamException("Height of neuron group is out of range: " + QString::number(height));
	spacing = (int)getParameter("spacing");
	if(spacing <=0)
		throw SpikeStreamException("Spacing of neurons is out of range: " + QString::number(spacing));
	density = getParameter("density");
	if(density < 0.0 || density > 1.0)
		throw SpikeStreamException("Density is out of range: " + QString::number(density));
}


