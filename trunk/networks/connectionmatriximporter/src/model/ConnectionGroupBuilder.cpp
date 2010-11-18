//SpikeStream includes
#include "ConnectionGroupBuilder.h"
#include "Globals.h"
#include "NetworkDao.h"
#include "SpikeStreamIOException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QFile>
#include <QTextStream>

//Other includes
#include <iostream>
using namespace std;

//Enable debuggging information
#define DEBUG


/*! Constructor */
ConnectionGroupBuilder::ConnectionGroupBuilder(QList<NeuronGroup*> excitNeurGrpList, QList<NeuronGroup*> inhibNeurGrpList){
	if(excitNeurGrpList.isEmpty() || inhibNeurGrpList.isEmpty())
		throw SpikeStreamException("Excitatory or neuron group list is empty.");
	if(excitNeurGrpList.size() != inhibNeurGrpList.size())
		throw SpikeStreamException("Excitatory neuron group size does not match inhibitory neuron group size.");

	//Store number of nodes
	this->excitNeurGrpList = excitNeurGrpList;
	this->inhibNeurGrpList = inhibNeurGrpList;
	numberOfNodes = excitNeurGrpList.size();

//	rng_t generator(42u);

//	  std::cout << "10 samples of a uniform distribution in [0..1):\n";

//	  // Define a uniform random number distribution which produces "double"
//	  // values between 0 and 1 (0 inclusive, 1 exclusive).
//	  boost::uniform_real<> uni_dist(0,1);
//	  boost::variate_generator<base_generator_type&, boost::uniform_real<> > uni(generator, uni_dist);


}


/*! Destructor */
ConnectionGroupBuilder::~ConnectionGroupBuilder(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

void ConnectionGroupBuilder::addConnectionGroups(Network* network, bool* stopThread, const QString& weightsFilePath, const QString& delaysFilePath, QHash<QString, double>& parameterMap){
	this->stopThread = stopThread;

	//Store the parameters as variables and keep parameter map for connection group parameters
	this->parameterMap = parameterMap;
	storeParameters();

	//Create synapse type for connections
	NetworkDao networkDao(Globals::getNetworkDao()->getDBInfo());
	synapseType = networkDao.getSynapseType("Izhikevich Synapse");

	emit progress(0, 1, "Loading connection data.");

	rng_t rng(42u);
	urng_t ranNumGen( rng, boost::uniform_real<double>(0, 1) );//Constructor of the random number generator

	//Load up the connections
	loadWeights(weightsFilePath);

	//Load up the delays
//	loadDelays(delaysFilePath);

	emit progress(1, 1, "Connection data loaded.");

	#ifdef DEBUG
		printConnections();
	#endif//DEBUG

	//Work through all of the neuron groups
	for(int nodeIdx=0; nodeIdx<excitNeurGrpList.size(); ++nodeIdx){
		addConnections(network, nodeIdx, ranNumGen);
	}
}





/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds excitatory and inhibitory connections to the network. */
void ConnectionGroupBuilder::addConnections(Network* network, int nodeIndex, urng_t& ranNumGen){
	//Local variables
	float tmpWeight;
	bool conGrpFound;
	unsigned toNeurID;

	//Get neuron groups
	NeuronGroup* excitNeurGrp = excitNeurGrpList.at(nodeIndex);
	NeuronGroup* inhibNeurGrp = inhibNeurGrpList.at(nodeIndex);

	//Add inhibitory connections within the node
	addInhibitoryConnections(network, excitNeurGrp, inhibNeurGrp);

	//Get list of the connections FROM the node TO other neuron groups
	QList<ConnectionInfo>& connectionList = neurGrpConList[nodeIndex];

	//Create inter node connection groups
	for(int i=0; i<connectionList.size(); ++i){
		ConnectionInfo& tmpConInfo = connectionList[i];
		NeuronGroup* fromNeurGrp = excitNeurGrpList[tmpConInfo.fromIndex];
		NeuronGroup* toNeurGrp = inhibNeurGrpList[tmpConInfo.toIndex];
		QString description = getConGrpDescription(fromNeurGrp, toNeurGrp);
		connectionList[i].connectionGroup = new ConnectionGroup(
				ConnectionGroupInfo(0, description, fromNeurGrp->getID(), toNeurGrp->getID(), parameterMap, synapseType) );
	}

	//Create intra node excitatory-excitatory connection group
	ConnectionGroup* excitExcitConGrp = new ConnectionGroup(ConnectionGroupInfo(0, getConGrpDescription(excitNeurGrp, excitNeurGrp), excitNeurGrp->getID(), excitNeurGrp->getID(), parameterMap, synapseType) );

	//Create the excitatory connections within and between nodes
	double randomNum, previousWeightsTotal;
	for(NeuronMap::iterator fromIter = excitNeurGrp->begin(); fromIter != excitNeurGrp->end() && !*stopThread; ++fromIter){
		for(NeuronMap::iterator toIter = excitNeurGrp->begin(); toIter != excitNeurGrp->end() && !*stopThread; ++toIter){
			//Get random number between 0 and 1.0
			randomNum = ranNumGen();

			//Get weight of connection
			tmpWeight = Util::getRandomFloat(minExcitatoryWeight, maxExcitatoryWeight);

			//Make excitatory connection within the from group
			if(randomNum < rewireThreshold){
				excitExcitConGrp->addConnection(fromIter.key(), toIter.key(), Util::getRandomUInt(minIntraDelay, maxIntraDelay), tmpWeight);
			}
			//Make connection to another group
			else{
				//Get a second random number between 0 and 1
				randomNum = ranNumGen();

				//Find the connection group whose weight falls within the range
				conGrpFound = false;
				previousWeightsTotal = 0.0;
				for(int conIdx=0; conIdx < connectionList.size(); ++conIdx){
					if(randomNum <= (connectionList.at(conIdx).weight + previousWeightsTotal) ){
						ConnectionInfo& tmpConInfo = connectionList[conIdx];
						toNeurID = getRandomExcitatoryNeuronID(tmpConInfo.toIndex);
						tmpConInfo.connectionGroup->addConnection(fromIter.key(), toNeurID, tmpConInfo.delay, tmpWeight);

						//Record that connection group has been found and exit loop.
						if(conGrpFound)
							throw SpikeStreamException("Connection group has been found twice - error somewhere.");
						conGrpFound = true;
						break;
					}
					qDebug()<<"from: "<<connectionList[conIdx].fromIndex<<"; to: "<<connectionList[conIdx].toIndex<<"; weight: "<<connectionList[conIdx].weight;
					qDebug()<<"Random number: "<<randomNum<<"; previousWeightsTotal: "<<previousWeightsTotal;

					//Add the current weight to previous weights
					previousWeightsTotal += connectionList.at(conIdx).weight;
				}
				if(!conGrpFound){
					throw SpikeStreamException("Threshold error. No connection group with threshold less than random number was found.");
				}
			}
		}
	}

	//Add connection groups with connections
	QList<ConnectionGroup*> newConGrpList;
	for(int i=0; i<connectionList.size(); ++i){
		if(connectionList.at(i).connectionGroup->size() == 0){
			qDebug()<<"Empty connection group: "<<connectionList.at(i).connectionGroup->getInfo().getDescription();
		}
		else{
			newConGrpList.append(connectionList.at(i).connectionGroup);
		}
	}
	if(excitExcitConGrp->size() > 0)
		newConGrpList.append(excitExcitConGrp);

	//Add connection groups to network
	network->addConnectionGroups(newConGrpList);
}


/*! Adds connections to and from inhibitory neuron groups.
	Only excitatory-excitatory connections are rewired. */
void ConnectionGroupBuilder::addInhibitoryConnections(Network* network, NeuronGroup* excitNeurGrp, NeuronGroup* inhibNeurGrp){
	//Local variables
	float tmpDelay, tmpWeight;

	//Create connection groups
	QList<ConnectionGroup*> newConGrpList;
	ConnectionGroup* excitInhibConGrp = new ConnectionGroup(ConnectionGroupInfo(0, getConGrpDescription(excitNeurGrp, inhibNeurGrp), excitNeurGrp->getID(), inhibNeurGrp->getID(), parameterMap, synapseType) );
	newConGrpList.append(excitInhibConGrp);
	ConnectionGroup* inhibExcitConGrp = new ConnectionGroup(ConnectionGroupInfo(0, getConGrpDescription(inhibNeurGrp, excitNeurGrp), inhibNeurGrp->getID(), excitNeurGrp->getID(), parameterMap, synapseType) );
	newConGrpList.append(inhibExcitConGrp);
	ConnectionGroup* inhibInhibConGrp = new ConnectionGroup(ConnectionGroupInfo(0, getConGrpDescription(inhibNeurGrp, inhibNeurGrp), inhibNeurGrp->getID(), inhibNeurGrp->getID(), parameterMap, synapseType) );
	newConGrpList.append(inhibInhibConGrp);

	//Add excitatory-inhibitory connections
	for(NeuronMap::iterator fromIter = excitNeurGrp->begin(); fromIter != excitNeurGrp->end() && !*stopThread; ++fromIter){
		for(NeuronMap::iterator toIter = inhibNeurGrp->begin(); toIter != inhibNeurGrp->end() && !*stopThread; ++toIter){
			tmpDelay = Util::getRandomUInt(minIntraDelay, maxIntraDelay);
			tmpWeight = Util::getRandomFloat(minExcitatoryWeight, maxExcitatoryWeight);
			excitInhibConGrp->addConnection(fromIter.key(), toIter.key(), tmpDelay, tmpWeight);
		}
	}

	//Add inhibitory-excitatory connections
	for(NeuronMap::iterator fromIter = inhibNeurGrp->begin(); fromIter != inhibNeurGrp->end() && !*stopThread; ++fromIter){
		for(NeuronMap::iterator toIter = excitNeurGrp->begin(); toIter != excitNeurGrp->end() && !*stopThread; ++toIter){
			tmpDelay = Util::getRandomUInt(minIntraDelay, maxIntraDelay);
			tmpWeight = Util::getRandomFloat(minInhibitoryWeight, maxInhibitoryWeight);
			inhibExcitConGrp->addConnection(fromIter.key(), toIter.key(), tmpDelay, tmpWeight);
		}
	}

	//Add inhibitory-inhibitory connections
	for(NeuronMap::iterator fromIter = excitNeurGrp->begin(); fromIter != excitNeurGrp->end() && !*stopThread; ++fromIter){
		for(NeuronMap::iterator toIter = inhibNeurGrp->begin(); toIter != inhibNeurGrp->end() && !*stopThread; ++toIter){
			tmpDelay = Util::getRandomUInt(minIntraDelay, maxIntraDelay);
			tmpWeight = Util::getRandomFloat(minInhibitoryWeight, maxInhibitoryWeight);
			inhibInhibConGrp->addConnection(fromIter.key(), toIter.key(), tmpDelay, tmpWeight);
		}
	}

	//Add connection groups to network
	network->addConnectionGroups(newConGrpList);
}


/*! Calculates the thresholds that are used to decide which connection group a rewired connection
	is placed in. */
void ConnectionGroupBuilder::calculateThresholds(){
	for(int i=0; i<neurGrpConList.size(); ++i){
		double nextThreshold = rewireThreshold;
		QList<ConnectionInfo>& conInfoList = neurGrpConList[i];
		for(int i=0; i<conInfoList.size(); ++i){
			conInfoList[i].threshold = nextThreshold;


			qDebug()<<"from: "<<conInfoList[i].fromIndex<<"; to: "<<conInfoList[i].toIndex<<"; weight: "<<conInfoList.at(i).weight;
			qDebug()<<"Rewire threshold: "<<rewireThreshold<<"; New threshold: "<<conInfoList[i].threshold<<"; next threshold: "<<nextThreshold;

			nextThreshold += (1.0 - rewireThreshold) * conInfoList.at(i).weight;
		}

		//Last threshold in this group should be less than the maximum
		if(!conInfoList.isEmpty() && (conInfoList.last().threshold >= 1.0 || conInfoList.last().threshold < rewireThreshold) )
			throw SpikeStreamException("Last connection threshold should be greater than rewireThreshold and less than 1.0: " + QString::number(conInfoList.last().threshold, 'g', 12));
	}
}


/*! Returns an automatically generated description of a connection between two neuron groups */
QString ConnectionGroupBuilder::getConGrpDescription(NeuronGroup* fromNeuronGroup, NeuronGroup* toNeuronGroup){
	return QString(fromNeuronGroup->getInfo().getName() + "->" + toNeuronGroup->getInfo().getName());
}


/*! Returns a neuron ID selected at random from the excitatory neuron group with the given index. */
unsigned ConnectionGroupBuilder::getRandomExcitatoryNeuronID(int index){
	if(index >= excitNeurGrpList.size())
		throw SpikeStreamException("Failed to get random neuron ID. Index out of range: " + QString::number(index));

	NeuronGroup* neurGrp = excitNeurGrpList.at(index);
	unsigned ranIndx = Util::getRandomUInt(0, neurGrp->size() - 1);
	return neurGrp->getNeuronMap()->values().at(ranIndx)->getID();
}


/*! Loads the delays from the delays file */
void ConnectionGroupBuilder::loadDelays(const QString& delaysFilePath){
	if(neurGrpConList.isEmpty())
		throw SpikeStreamException("No connections loaded. Delays must be loaded after the connection matrix.");

	//Check file exists and open it
	QFile delaysFile(delaysFilePath);
	if(!delaysFile.exists())
		throw SpikeStreamIOException("Cannot find delays file.");
	if (!delaysFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open delays file for reading: " + delaysFile.fileName());

	//Load delays from file in same order as for weights
	int rowCntr = 0;
	QTextStream in(&delaysFile);
	QString line;
	while (!in.atEnd()) {
		line = in.readLine();
		if(!line.isEmpty()){
			QStringList strList = line.split(QRegExp("\\s+"));
			if(strList.size() != numberOfNodes)
				throw SpikeStreamException("Number of nodes in delays file does not equal number of nodes loaded: " + numberOfNodes);

			//Work through all the FROM indexes connecting TO the connection group represented by the row.
			int conCtr = 0;
			for(int i=0; i<strList.size(); ++i){
				double tmpDelay = Util::getDouble(strList.at(i));
				if(tmpDelay != 0.0){
					if(conCtr >= neurGrpConList[i].size())
						throw SpikeStreamException("Delay connection index: " + QString::number(conCtr) + " >= number of connections loaded from connection matrix: " + QString::number(neurGrpConList[i].size()) );
					neurGrpConList[i][conCtr].delay = tmpDelay;
					++conCtr;
				}
				if(conCtr != neurGrpConList[i].size())
					throw SpikeStreamException("Mismatch between number of delay connections " + QString::number(conCtr) + " and number of connections loaded from connection matrix: " + QString::number(neurGrpConList[i].size()) );
			}
			++rowCntr;
		}
	}

	//Check row count is correct
	if(rowCntr != numberOfNodes)
		throw SpikeStreamException("Row count does not match number of nodes: " + QString::number(rowCntr));

	//Close file
	delaysFile.close();
}


/*! Loads the weights up from the connection matrix file */
void ConnectionGroupBuilder::loadWeights(const QString& weightsFilePath){
	//Fill list with appropriate number of lists
	neurGrpConList.clear();
	for(int i=0; i<numberOfNodes; ++i)
		neurGrpConList.append(QList<ConnectionInfo>());

	//Check file exists and open it
	QFile weightsFile(weightsFilePath);
	if(!weightsFile.exists())
		throw SpikeStreamIOException("Cannot find connection matrix file.");
	if (!weightsFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open connection matrix file for reading: " + weightsFile.fileName());

	//Load weights from file
	int rowCntr = 0;
	QTextStream in(&weightsFile);
	QString line;
	while (!in.atEnd()) {
		line = in.readLine();
		if(!line.isEmpty()){
			QStringList strList = line.split(QRegExp("\\s+"));
			if(strList.size() != numberOfNodes)
				throw SpikeStreamException("Number of nodes in connection matrix does not equal number of nodes loaded: " + numberOfNodes);

			//Work through all the FROM indexes connecting TO the connection group represented by the row.
			for(int i=0; i<strList.size(); ++i){
				float tmpWeight = Util::getFloat(strList.at(i));
				if(tmpWeight != 0.0){
					neurGrpConList[i].append(ConnectionInfo(i, rowCntr, 1, tmpWeight));
				}
			}
			++rowCntr;
		}
	}

	//Check row count is correct
	if(rowCntr != numberOfNodes)
		throw SpikeStreamException("Row count does not match number of nodes: " + QString::number(rowCntr));

	//Close file
	weightsFile.close();

	//Normalize the weights and calculate the thresholds
	normalizeWeights();
	//calculateThresholds();
}


/*! Normalizes weights so that the maximum possible weight is 1 */
void ConnectionGroupBuilder::normalizeWeights(){
	//Local variables
	double weightMax = 0.0, weightFactor;

	//Find the maximum weight
	for(int i=0; i<neurGrpConList.size(); ++i){
		QList<ConnectionInfo>& conInfoList = neurGrpConList[i];
		for(int j=0; j<conInfoList.size(); ++j){
			if(conInfoList.at(j).weight > weightMax)
				weightMax = conInfoList.at(j).weight;
		}
	}

	//Multiply all weights by weight factor so that the maximum becomes 1.0
	weightFactor = 1.0 / weightMax;
	for(int i=0; i<neurGrpConList.size(); ++i){//All nodes
		QList<ConnectionInfo>& conInfoList = neurGrpConList[i];
		for(int j=0; j<conInfoList.size(); ++j){//All connections
			conInfoList[j].weight *= weightFactor;
		}
	}
}


/*! Prints out the connections for debugging. */
void ConnectionGroupBuilder::printConnections(bool printToFile){
	if(printToFile){
		QFile logFile(Globals::getSpikeStreamRoot() + "/log/ConnectionGroupBuilder.log");
		if(!logFile.open(QFile::WriteOnly | QFile::Truncate))
			throw SpikeStreamIOException("Cannot open log file for NemoLoader.");
		QTextStream logOut(&logFile);
		logOut<<"---------------------  CONNECTIONS -------------------"<<endl;
		for(int i=0; i<neurGrpConList.size(); ++i){
			logOut<<"Neuron: "<<i<<" Connections."<<endl;
			QList<ConnectionInfo>& conInfoList = neurGrpConList[i];
			for(int j=0; j<conInfoList.size(); ++j){
				ConnectionInfo& tmpCon = conInfoList[j];
				logOut<<"\tFrom index: "<<tmpCon.fromIndex<<"; to index: "<<tmpCon.toIndex<<"; delay: "<<tmpCon.delay;
				logOut<<"; weight: "<<tmpCon.weight<<"; threshold: "<<tmpCon.threshold<<endl;
			}
		}
		logFile.close();
		return;
	}
	cout<<"---------------------  CONNECTIONS -------------------"<<endl;
	for(int i=0; i<neurGrpConList.size(); ++i){
		cout<<"Neuron: "<<i<<" Connections."<<endl;
		QList<ConnectionInfo>& conInfoList = neurGrpConList[i];
		for(int j=0; j<conInfoList.size(); ++j){
			ConnectionInfo& tmpCon = conInfoList[j];
			cout<<"\tFrom index: "<<tmpCon.fromIndex<<"; to index: "<<tmpCon.toIndex<<"; delay: "<<tmpCon.delay;
			cout<<"; weight: "<<tmpCon.weight<<"; threshold: "<<tmpCon.threshold<<endl;
		}
	}
}


/*! Stores the parameters required by this class */
void ConnectionGroupBuilder::storeParameters(){
	double rewireProb = Util::getDoubleParameter("rewire_probability", parameterMap) ;
	if(rewireProb < 0.0 || rewireProb > 1.0)
		throw SpikeStreamException("Rewire probability out of range: " + QString::number(rewireProb) + ". It must be between 0.0 and 1.0 inclusive.");
	rewireThreshold = 1.0 - rewireProb;

	minIntraDelay = Util::getUIntParameter("min_intra_delay", parameterMap);
	maxIntraDelay = Util::getUIntParameter("max_intra_delay", parameterMap);

	minExcitatoryWeight = Util::getFloatParameter("min_excitatory_weight", parameterMap);
	if(minExcitatoryWeight < -1.0 || minExcitatoryWeight > 1.0 )
		throw SpikeStreamException("minExcitatoryWeight out of range: " + QString::number(minExcitatoryWeight) + ". It must be between -1.0 and 1.0 inclusive.");

	maxExcitatoryWeight = Util::getFloatParameter("max_excitatory_weight", parameterMap);
	if(maxExcitatoryWeight < -1.0 || maxExcitatoryWeight > 1.0 )
		throw SpikeStreamException("maxExcitatoryWeight out of range: " + QString::number(maxExcitatoryWeight) + ". It must be between -1.0 and 1.0 inclusive.");

	minInhibitoryWeight = Util::getFloatParameter("min_inhibitory_weight", parameterMap);
	if(minInhibitoryWeight < -1.0 || minInhibitoryWeight > 1.0 )
		throw SpikeStreamException("minInhibitoryWeight out of range: " + QString::number(minInhibitoryWeight) + ". It must be between -1.0 and 1.0 inclusive.");

	maxInhibitoryWeight = Util::getFloatParameter("max_inhibitory_weight", parameterMap);
	if(maxInhibitoryWeight < -1.0 || maxInhibitoryWeight > 1.0 )
		throw SpikeStreamException("maxInhibitoryWeight out of range: " + QString::number(maxInhibitoryWeight) + ". It must be between -1.0 and 1.0 inclusive.");
}



