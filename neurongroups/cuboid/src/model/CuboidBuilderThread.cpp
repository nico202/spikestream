//SpikeStream includes
#include "CuboidBuilderThread.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
CuboidBuilderThread::CuboidBuilderThread(){
}


/*! Destructor */
CuboidBuilderThread::~CuboidBuilderThread(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Prepares class before it runs as a separate thread to add one or more neuron groups */
void CuboidBuilderThread::prepareAddNeuronGroups(const QString& name, const QString& description, QHash<QString, double>& paramMap){
	//Run some checks
	if(!Globals::networkLoaded())
		throw SpikeStreamException("Cannot add neuron group - no network loaded.");
	if(Globals::getNetwork()->hasArchives())
		throw SpikeStreamException("Cannot add neuron group to a locked network.\nDelete archives linked with this network and try again");

	//Create the neuron groups to be added. A separate neuron group is added for each neuron type
	createNeuronGroups(name, description, paramMap);

	//Extract parameters from neuron group
	storeParameters(paramMap);
}


/*! Thread run method */
void CuboidBuilderThread::run(){
	clearError();
	stopThread = false;
	try{
		//Seed the random number generator
		srand(12345678);

		//Create network and archive dao for this thread
		threadNetworkDao = new NetworkDao(Globals::getNetworkDao()->getDBInfo());
		threadArchiveDao = new ArchiveDao(Globals::getArchiveDao()->getDBInfo());

		//Need to set a new network and archive dao in the network because we are running as a separate thread
		Network* currentNetwork = Globals::getNetwork();

		//Add the neuron groups to the database
		addNeuronGroupsToDatabase();

		//Wait for network to finish
		while(currentNetwork->isBusy()){
			emit progress(threadNetworkDao->getNeuronCount(newNeuronGroupList), totalNumberOfNeurons, "Adding neurons to database...");
			msleep(250);
		}

		//Check for errors
		if(currentNetwork->isError())
			setError(currentNetwork->getErrorMessage());

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
}


/*! Stops the thread running  */
void CuboidBuilderThread::stop(){
	stopThread = true;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds a neuron group with the specified parameters to the database */
void CuboidBuilderThread::addNeuronGroupsToDatabase(){
	//Add the neurons to the neuron groups
	addNeurons();

	//Add the neuron groups to the network
	Globals::getNetwork()->addNeuronGroups(newNeuronGroupList);
}



/*! Clears error state and message */
void CuboidBuilderThread::clearError(){
	error = false;
	errorMessage = "";
}


/*! Creates a neuron group for each type of neuron.
	NOTE: Only call this within the prepare method. */
void CuboidBuilderThread::createNeuronGroups(const QString& name, const QString& description, QHash<QString, double>& paramMap){
	//Reset maps
	newNeuronGroupMap.clear();
	neuronTypePercentThreshMap.clear();

	//Get all the available neuron types
	QList<NeuronType> neurTypeList = Globals::getNetworkDao()->getNeuronTypes();
	double total = 0.0;
	foreach(NeuronType neurType, neurTypeList){
		unsigned int neurTypeID = neurType.getID();
		if(paramMap.contains("percent_neuron_type_id_" + QString::number(neurTypeID))){
			total += paramMap["percent_neuron_type_id_" + QString::number(neurTypeID)];
			neuronTypePercentThreshMap[total] = neurTypeID;
			newNeuronGroupMap[neurTypeID] = new NeuronGroup(NeuronGroupInfo(0, name, description, paramMap, neurTypeID));
		}
	}

	//Check for errors
	if(newNeuronGroupMap.isEmpty()){
		Util::printParameterMap(paramMap);
		throw SpikeStreamException("Neuron type error - no neuron types found to load.");
	}
}


/*! Returns a neuron group whose neurons are constructed according to the
	parameters in the neuron group info. */
void CuboidBuilderThread::addNeurons(){
	int cntr = 0;
	for(int xPos = xStart; xPos < xStart+width && !stopThread; xPos += spacing){
		for(int yPos = yStart; yPos < yStart+length && !stopThread; yPos += spacing){
			for(int zPos = zStart; zPos < zStart+height && !stopThread; zPos += spacing){
				//Random number for density
				double ranNumDensity = (double)rand() / (double)RAND_MAX;

				if(ranNumDensity <= density){
					//Random number for type of neuron
					double ranNumPercent = 100.0 * ( (double)rand() / (double)RAND_MAX );

					QMap<double, unsigned int>::iterator neurTypePercentThreshMapEnd = neuronTypePercentThreshMap.end();
					for(QMap<double, unsigned int>::iterator iter = neuronTypePercentThreshMap.begin(); iter != neurTypePercentThreshMapEnd; ++iter){
						//Add neuron if the random number is less than the threshold
						if(ranNumPercent <= iter.key()){
							newNeuronGroupMap[iter.value()]->addNeuron(xPos, yPos, zPos);
							break;
						}
					}
				}
				++cntr;
			}
		}
	}

	//Convert map to list format and sum total number of neurons
	newNeuronGroupList.clear();
	totalNumberOfNeurons = 0;
	for(QHash<unsigned int, NeuronGroup*>::iterator iter = newNeuronGroupMap.begin(); iter != newNeuronGroupMap.end(); ++iter){
		newNeuronGroupList.append(iter.value());
		totalNumberOfNeurons += iter.value()->size();
	}
}


/*! Returns a parameter from the neuron group info parameter map checking that it actually exists */
double CuboidBuilderThread::getParameter(const QString& paramName, const QHash<QString, double>& paramMap){
	if(!paramMap.contains(paramName))
		throw SpikeStreamException("Parameter with " + paramName + " does not exist in parameter map.");
	return paramMap[paramName];
}


/*! Prints a summary of the neuron group(s) that have been created. */
void CuboidBuilderThread::printSummary(){
	cout<<newNeuronGroupMap.size()<<" neuron groups added. Width: "<<width<< " length: "<<length<<" height: "<<height<<endl;
	for(QHash<unsigned int, NeuronGroup*>::iterator iter = newNeuronGroupMap.begin(); iter != newNeuronGroupMap.end(); ++iter){
		cout<<"Neuron type "<<iter.key()<<" added "<<iter.value()->size()<<" neurons."<<endl;
	}
}


/*! Puts the thread into error state and stores the message */
void CuboidBuilderThread::setError(const QString& errorMessage){
	error = true;
	this->errorMessage = errorMessage;
	stopThread = true;
}


/*! Extracts parameters from neuron group info and stores them in the appropriate format */
void CuboidBuilderThread::storeParameters(const QHash<QString, double>& paramMap){
	xStart = (int)getParameter("x", paramMap);
	yStart = (int)getParameter("y", paramMap);
	zStart = (int)getParameter("z", paramMap);
	width = (int)getParameter("width", paramMap);
	if(width <=0)
		throw SpikeStreamException("Width of neuron group is out of range: " + QString::number(width));
	length = (int)getParameter("length", paramMap);
	if(length <=0)
		throw SpikeStreamException("Length of neuron group is out of range: " + QString::number(length));
	height = (int)getParameter("height", paramMap);
	if(height <=0)
		throw SpikeStreamException("Height of neuron group is out of range: " + QString::number(height));
	spacing = (int)getParameter("spacing", paramMap);
	if(spacing <=0)
		throw SpikeStreamException("Spacing of neurons is out of range: " + QString::number(spacing));
	density = getParameter("density", paramMap);
	if(density < 0.0 || density > 1.0)
		throw SpikeStreamException("Density is out of range: " + QString::number(density));
}


