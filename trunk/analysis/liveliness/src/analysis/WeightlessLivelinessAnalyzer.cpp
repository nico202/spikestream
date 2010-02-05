//SpikeStream includes
#include "Util.h"
#include "WeightlessLivelinessAnalyzer.h"
using namespace spikestream;


/*! Constructor */
WeightlessLivelinessAnalyzer::WeightlessLivelinessAnalyzer(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo, const AnalysisInfo& anaInfo, unsigned int timeStep);
	//Store variables
	this->analysisInfo = anaInfo;
	this->timeStep = timeStep;

	//Create data access objects
	networkDao = new NetworkDao(netDBInfo);
	archiveDao = new ArchiveDao(archDBInfo);
	livelinessDao = new LivelinessDao(anaDBInfo);
}


/*! Destructor */
WeightlessLivelinessAnalyzer::~WeightlessLivelinessAnalyzer(){

}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Runs the analysis for liveliness on the network */
void WeightlessLivelinessAnalyzer::runCalculation(const bool * const stop){
	//Load up weightless neurons, firing neurons and connections
	loadWeightlessNeurons();
	loadFiringNeurons();
	networkDao->getAllToConnections(analysisInfo.getNetworkID(), toConnectionMap);

	//Work through the stages in the analysis
	calculateConnectionLiveliness();
	calculateNeuronLiveliness();
	identifyClusters();
}


/*-------------------------------------------------------------*/
/*-------                 PRIVATE METHODS                ------*/
/*-------------------------------------------------------------*/

/*! Identifies the liveliness of each connection and optionally stores it in the tmp
	weight field of the network database */
void WeightlessLivelinessAnalyzer::calculateConnectionLiveliness(){
	//Work through each weightless neuron
	for(QHash<unsigned int, WeightlessNeuron*>::iterator weiNeurIter = weightlessNeuronMap.begin(); weiNeurIter != weightlessNeuronMap.end(); ++weiNeurIter){
		WeightlessNeuron* tmpWeiNeur = weiNeurIter.value();
		QHash<unsigned int, QList<unsigned int> > tmpConMap = tmpWeiNeur->getConnectionMap();

		//Create input byte array that reflects the firing state of the connected neurons
		byte* inPatArr;
		int inPatArrLen;
		fillInputArray(tmpWeiNeur, inPatArr, inPatArrLen);

		//Work through all the connections to the neuron
		for(QHash<unsigned int, QList<unsigned int> >::iterator conIter = tmpConMap.begin(); conIter != tmpConMap.end(); ++conIter){

			//Get firing probability with neuron in its current state. Firing state is set to 1, but could equally well be zero
			double firingProb1 = tmpWeiNeur->getFiringStateProbability(inPatArr, inPatArrLen, 1);

			//Flip the bit corresponding to the neuron under test
			flipBits(inPatArr, iter.value());

			//Get probability again
			double firingProb2 = tmpWeiNeur->getFiringStateProbability(inPatArr[], inPatArrLen, 1);

			//If probability has changed, neuron is lively
			if(firingProb1 != firingProb2){
				setConnectionLiveliness(conIter.key(), tmpWeiNeur->getID(), 1.0);
			}
			else{
				setConnectionLiveliness(conIter.key(), tmpWeiNeur->getID(), 0.0);
			}

			//Flip bit back ready to test next neuron in connection set
			flipBits(inPatArr, inPatArrLen, iter.value());
		}

		//Clean up array
		delete [] inPatArr;
	}
}

void WeightlessLivelinessAnalyzer::calculateNeuronLiveliness(){
}


/*! Creates an array and populates it with the firing pattern of neurons that connect to the
	specified weightless neuron. */
void WeightlessLivelinessAnalyzer::fillInputArray(WeightlessNeuron* weiNeuron, byte* inPatArr, int& inPatArrLen){
	//Calculate the length of the array
	if(weiNeuron->getNumberOfConnections() % 8 == 0)
		inPatArrLen = weiNeuron->getNumberOfConnections() / 8;
	else
		inPatArrLen = weiNeuron->getNumberOfConnections() / 8 + 1;

	//Create pattern array and initialize with zeros
	inPatArr = new byte [inPatArrLen];
	for(int i=0; i<inPatArrLen; ++i)
		inPatArr[i] = 0;

	//Fill input array according to firing patterns of other neurons
	QHash<unsigned int, QList<unsigned int> > conMap = weiNeuron->getConnectionMap();
	for(QHash<unsigned int, QList<unsigned int> >::iterator conIter = conMap.begin(); conIter != conMap.end(); ++conIter){
		//Connected neuron is firing
		if(firingNeuronMap.contains(conIter.key())){
			//Set all indexes associated with this neuron to 1
			foreach(unsigned int indx, conNeurIter.value()){
				if(indx / 8 > inPatArrSize)
					throw SpikeStreamAnalysisException("Input pattern array index out of range.");
				inPatArr[ indx/8 ] |= 1<<(indx % 8);
			}
		}
	}
}


/*! Flips the bits corresponding to the indexes associated with the supplied neuron */
void WeightlessLivelinessAnalyzer::flipBits(byte inPatArr [], int inPatArrLen, unsigned int neurID){
	QHash<unsigned int, QList<unsigned int> > conMap = weiNeuron->getConnectionMap();

	if(!conMap.contains(neurID))
		throw SpikeStreamAnalysisException("Neuron ID not found in connection map");

	foreach(unsigned int indx, conMap[neurID]){
		inPatArr[ indx/8 ] ^= 1<<(indx % 8);
	}
}


void WeightlessLivelinessAnalyzer::identifyClusters(){
}


/*! Deletes all of the weightless neurons currently stored in this class */
void WeightlessLivelinessAnalyzer::deleteWeightlessNeurons(){
	for(QHash<unsigned int, WeightlessNeuron*>::iterator iter = weightlessNeuronMap.begin(); iter != weightlessNeuronMap.end(); ++iter){
		delete iter.value();
	}
	weightlessNeuronMap.clear();
}


/*! Loads up the neurons firing at this time step. */
void WeightlessLivelinessAnalyzer::loadFiringNeurons(){
	if(archiveDao == NULL){
		throw SpikeStreamAnalysisException("Archive dao has not been set. Empty constructor should only be used for unit testing.");
	}

	firingNeuronMap.clear();
	QStringList neurIDStrList = archiveDao->getFiringNeuronIDs(analysisInfo.getArchiveID(), timeStep);
	QStringListIterator iter(neurIDStrList);
	while (iter.hasNext()){
		firingNeuronMap[ Util::getUInt(iter.next()) ] = true;
	}
}


/*! Loads up all of the weightless neurons */
void WeightlessLivelinessAnalyzer::loadWeightlessNeurons(){
	if(networkDao == NULL){
		throw SpikeStreamAnalysisException("Network dao has not been set. Empty constructor should only be used for unit testing.");
	}

	deleteWeightlessNeurons();
	QList<unsigned int> neuronIDList = networkDao->getNeuronIDs(analysisInfo.getNetworkID());
	foreach(unsigned int neurID, neuronIDList){
		//Store neuron
		weightlessNeuronMap[neurID] = networkDao->getWeightlessNeuron(neurID);

		//Set parameters in neuron
		weightlessNeuronMap[neurID]->setGeneralization(analysisInfo.getParameter("generalization"));
	}
}


void WeightlessLivelinessAnalyzer::setConnectionLiveliness(unsigned int fromNeurID, unsigned int toNeurID, double liveliness){
}





