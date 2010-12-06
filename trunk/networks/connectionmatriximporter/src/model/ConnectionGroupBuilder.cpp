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
//#define DEBUG


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

	//Load up default parameters for the available synapse types
	loadDefaultParameters();
}


/*! Destructor */
ConnectionGroupBuilder::~ConnectionGroupBuilder(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

void ConnectionGroupBuilder::addConnectionGroups(Network* network, bool* stopThread, const QString& weightsFilePath, const QString& delaysFilePath, QHash<QString, double>& parameterMap, urng_t& ranNumGen){
	this->stopThread = stopThread;

	//Store the parameters as variables and keep parameter map for connection group parameters
	this->parameterMap = parameterMap;
	storeParameters();

	//Create synapse type for connections
	NetworkDao networkDao(Globals::getNetworkDao()->getDBInfo());
	synapseType = networkDao.getSynapseType("Izhikevich Synapse");

	//Check we have default parameters
	if(!defaultParameterMaps.contains(synapseType.getID()))
		throw SpikeStreamException("Default parameter map(s) missing for synapse type.");

	emit progress(0, 1, "Loading connection data.");

	//Load up the connections
	loadWeights(weightsFilePath);

	//Load up the delays
	loadDelays(delaysFilePath);

	if(rewireToConnections)
		calculateToConnectionList();

	emit progress(1, 1, "Connection data loaded.");

	#ifdef DEBUG
		printConnections();
	#endif//DEBUG

	//Work through all of the neuron groups
	for(int nodeIdx=0; nodeIdx<excitNeurGrpList.size(); ++nodeIdx){
		emit progress(nodeIdx, excitNeurGrpList.size()-1, "Adding connections...");
		addConnections(network, nodeIdx, ranNumGen);
	}
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds excitatory and inhibitory connections to the network.
	Rewiring takes place of the connections FROM each neuron. */
void ConnectionGroupBuilder::addConnections(Network* network, int nodeIndex, urng_t& ranNumGen){
	//Local variables
	bool conGrpFound;
	unsigned fromNeurID, toNeurID;

	//Get neuron groups
	NeuronGroup* excitNeurGrp = excitNeurGrpList.at(nodeIndex);
	NeuronGroup* inhibNeurGrp = inhibNeurGrpList.at(nodeIndex);

	//Get list of the connections FROM the node TO other neuron groups
	QList<ConnectionInfo>& connectionList = neurGrpConList[nodeIndex];
	if(rewireToConnections)
		connectionList = toNeurGrpConList[nodeIndex];

	//Add inhibitory connections within the node
	addInhibitoryConnections(network, excitNeurGrp, inhibNeurGrp, ranNumGen);

	//Create inter node connection groups
	for(int i=0; i<connectionList.size(); ++i){
		ConnectionInfo& tmpConInfo = connectionList[i];
		NeuronGroup* fromNeurGrp = excitNeurGrpList[tmpConInfo.fromIndex];
		NeuronGroup* toNeurGrp = excitNeurGrpList[tmpConInfo.toIndex];
		QString description = getConGrpDescription(fromNeurGrp, toNeurGrp);
		connectionList[i].connectionGroup = new ConnectionGroup(
				ConnectionGroupInfo(0, description, fromNeurGrp->getID(), toNeurGrp->getID(), parameterMap, synapseType) );
		connectionList[i].connectionGroup->setParameters(defaultParameterMaps[synapseType.getID()]);
	}

	//Create intra node excitatory-excitatory connection group
	ConnectionGroup* excitExcitConGrp = new ConnectionGroup(
			ConnectionGroupInfo(0, getConGrpDescription(excitNeurGrp, excitNeurGrp),
			excitNeurGrp->getID(), excitNeurGrp->getID(), parameterMap, synapseType) );
	excitExcitConGrp->setParameters(defaultParameterMaps[synapseType.getID()]);

	//Create the excitatory connections within and between nodes
	double randomNum, previousThreshold;
	for(NeuronMap::iterator fromIter = excitNeurGrp->begin(); fromIter != excitNeurGrp->end() && !*stopThread; ++fromIter){
		for(NeuronMap::iterator toIter = excitNeurGrp->begin(); toIter != excitNeurGrp->end() && !*stopThread; ++toIter){
			//Get random number between 0 and 1.0
			randomNum = ranNumGen();

			//See if random number lies within the range of one of the connection weights
			conGrpFound = false;
			previousThreshold = 0.0;
			for(int conIdx=0; conIdx < connectionList.size(); ++conIdx){//Work through all connections from this neuron
				if(randomNum < (connectionList.at(conIdx).threshold + previousThreshold) ){//Does the random number fall within range of this weight?
					//Create connection
					ConnectionInfo& tmpConInfo = connectionList[conIdx];
					if(rewireToConnections){
						fromNeurID = getRandomExcitatoryNeuronID(tmpConInfo.fromIndex);
						tmpConInfo.connectionGroup->addConnection(fromNeurID, toIter.key(), getInterDelay(ranNumGen(), tmpConInfo.delay), getExcitatoryWeight(ranNumGen()));
					}
					else{
						toNeurID = getRandomExcitatoryNeuronID(tmpConInfo.toIndex);
						tmpConInfo.connectionGroup->addConnection(fromIter.key(), toNeurID, getInterDelay(ranNumGen(), tmpConInfo.delay), getExcitatoryWeight(ranNumGen()));
					}

					//Record that connection group has been found and exit loop.
					conGrpFound = true;
					break;
				}

				//Add the current weight to previous weights
				previousThreshold += connectionList.at(conIdx).threshold;
			}

			//Create intra group connection if we have not rewired.
			if(!conGrpFound){
				excitExcitConGrp->addConnection(fromIter.key(), toIter.key(), getIntraDelay(ranNumGen()), getExcitatoryWeight(ranNumGen()));
			}
		}
	}

	//Add connection groups with connections
	QList<ConnectionGroup*> newConGrpList;
	for(int i=0; i<connectionList.size(); ++i){
		if(connectionList.at(i).connectionGroup->size() == 0){
			#ifdef DEBUG
				qDebug()<<"Empty connection group: "<<connectionList.at(i).connectionGroup->getInfo().getDescription();
			#endif//DEBUG
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
void ConnectionGroupBuilder::addInhibitoryConnections(Network* network, NeuronGroup* excitNeurGrp, NeuronGroup* inhibNeurGrp, urng_t& ranNumGen){
	//Create connection groups
	QList<ConnectionGroup*> newConGrpList;
	ConnectionGroup* excitInhibConGrp = new ConnectionGroup(ConnectionGroupInfo(0, getConGrpDescription(excitNeurGrp, inhibNeurGrp), excitNeurGrp->getID(), inhibNeurGrp->getID(), parameterMap, synapseType) );
	excitInhibConGrp->setParameters(defaultParameterMaps[synapseType.getID()]);
	newConGrpList.append(excitInhibConGrp);

	ConnectionGroup* inhibExcitConGrp = new ConnectionGroup(ConnectionGroupInfo(0, getConGrpDescription(inhibNeurGrp, excitNeurGrp), inhibNeurGrp->getID(), excitNeurGrp->getID(), parameterMap, synapseType) );
	inhibExcitConGrp->setParameters(defaultParameterMaps[synapseType.getID()]);
	newConGrpList.append(inhibExcitConGrp);

	ConnectionGroup* inhibInhibConGrp = new ConnectionGroup(ConnectionGroupInfo(0, getConGrpDescription(inhibNeurGrp, inhibNeurGrp), inhibNeurGrp->getID(), inhibNeurGrp->getID(), parameterMap, synapseType) );
	inhibInhibConGrp->setParameters(defaultParameterMaps[synapseType.getID()]);
	newConGrpList.append(inhibInhibConGrp);

	//Add excitatory-inhibitory connections
	for(NeuronMap::iterator fromIter = excitNeurGrp->begin(); fromIter != excitNeurGrp->end() && !*stopThread; ++fromIter){
		for(NeuronMap::iterator toIter = inhibNeurGrp->begin(); toIter != inhibNeurGrp->end() && !*stopThread; ++toIter){
			excitInhibConGrp->addConnection(fromIter.key(), toIter.key(), getIntraDelay(ranNumGen()), getExcitatoryWeight(ranNumGen()));
		}
	}

	//Add inhibitory-excitatory connections
	for(NeuronMap::iterator fromIter = inhibNeurGrp->begin(); fromIter != inhibNeurGrp->end() && !*stopThread; ++fromIter){
		for(NeuronMap::iterator toIter = excitNeurGrp->begin(); toIter != excitNeurGrp->end() && !*stopThread; ++toIter){
			inhibExcitConGrp->addConnection(fromIter.key(), toIter.key(), getIntraDelay(ranNumGen()), getInhibitoryWeight(ranNumGen()));
		}
	}

	//Add inhibitory-inhibitory connections
	for(NeuronMap::iterator fromIter = inhibNeurGrp->begin(); fromIter != inhibNeurGrp->end() && !*stopThread; ++fromIter){
		for(NeuronMap::iterator toIter = inhibNeurGrp->begin(); toIter != inhibNeurGrp->end() && !*stopThread; ++toIter){
			inhibInhibConGrp->addConnection(fromIter.key(), toIter.key(), getIntraDelay(ranNumGen()), getInhibitoryWeight(ranNumGen()));
		}
	}

	//Add connection groups to network
	network->addConnectionGroups(newConGrpList);
}


/*! Calculates the connections to each node. They are loaded from each node. */
void ConnectionGroupBuilder::calculateToConnectionList(){
	//Build the list - same number of nodes as from list
	for(int nodeIdx=0; nodeIdx<neurGrpConList.size(); ++nodeIdx){
		toNeurGrpConList.append(QList<ConnectionInfo>());
	}

	//Populate list
	for(int nodeIdx=0; nodeIdx<neurGrpConList.size(); ++nodeIdx){
		QList<ConnectionInfo>& conInfoList = neurGrpConList[nodeIdx];
		for(int conIdx=0; conIdx<conInfoList.size(); ++conIdx){
			ConnectionInfo& tmpConInfo = conInfoList[conIdx];
			toNeurGrpConList[tmpConInfo.toIndex].append(tmpConInfo);
		}
	}
}


/*! Calculates threshold that decides whether rewiring takes place.
	This makes sure that the correct proportion of rewired connection is allocated to each connection. */
void ConnectionGroupBuilder::calculateThresholds(){
	//Get the total number of excitatory connections
	double excitTot = 0.0;
	for(int i=0; i<excitNeurGrpList.size(); ++i)
		excitTot += excitNeurGrpList.at(i)->size() * excitNeurGrpList.at(i)->size();

	//Threshold is the number of rewired connections divided by the total number of excitatory connections
	double numExcitatoryCons;
	for(int nodeIdx=0; nodeIdx<neurGrpConList.size(); ++nodeIdx){
		numExcitatoryCons = excitNeurGrpList.at(nodeIdx)->size() * excitNeurGrpList.at(nodeIdx)->size();
		QList<ConnectionInfo>& conInfoList = neurGrpConList[nodeIdx];
		for(int conIdx=0; conIdx<conInfoList.size(); ++conIdx){
			conInfoList[conIdx].threshold = (conInfoList[conIdx].weight * excitTot) / numExcitatoryCons;
		}
	}
}


/*! Returns an automatically generated description of a connection between two neuron groups */
QString ConnectionGroupBuilder::getConGrpDescription(NeuronGroup* fromNeuronGroup, NeuronGroup* toNeuronGroup){
	QString fromGrpStr = fromNeuronGroup->getInfo().getName();
	if(fromNeuronGroup->getInfo().getNeuronType().getDescription().contains("Excitatory", Qt::CaseInsensitive))
		fromGrpStr += " (Excit)";
	else if(fromNeuronGroup->getInfo().getNeuronType().getDescription().contains("Inhibitory", Qt::CaseInsensitive))
		fromGrpStr += " (Inhib)";
	else
		throw SpikeStreamException("Neuron type not recognized.");
	QString toGrpStr = toNeuronGroup->getInfo().getName();
	if(toNeuronGroup->getInfo().getNeuronType().getDescription().contains("Excitatory", Qt::CaseInsensitive))
		toGrpStr += " (Excit)";
	else if(toNeuronGroup->getInfo().getNeuronType().getDescription().contains("Inhibitory", Qt::CaseInsensitive))
		toGrpStr += " (Inhib)";
	else
		throw SpikeStreamException("Neuron type not recognized.");
	return QString(fromGrpStr + "->" + toGrpStr);
}


/*! Returns an excitatory weight within the specified range. */
float ConnectionGroupBuilder::getExcitatoryWeight(double randomNum){
	return randomNum * (maxExcitatoryWeight - minExcitatoryWeight) + minExcitatoryWeight;
}


/*! Returns an inhibitory weight within the specified range. */
float ConnectionGroupBuilder::getInhibitoryWeight(double randomNum){
	return randomNum * (maxInhibitoryWeight - minInhibitoryWeight) + minInhibitoryWeight;
}

/*! Returns a delay for a connection between connection groups.
	Random number should be between 0 and 1 */
float ConnectionGroupBuilder::getInterDelay(double randomNum, double connectionDelay){
	return connectionDelay + getIntraDelay(randomNum);
}


/*! Returns a delay for a connection within a connection group.
	Random number should be between 0 and 1. */
float ConnectionGroupBuilder::getIntraDelay(double randomNum){
	return randomNum * (maxDelayRange - minDelayRange) + minDelayRange;
}


/*! Returns a neuron ID selected at random from the excitatory neuron group with the given index. */
unsigned ConnectionGroupBuilder::getRandomExcitatoryNeuronID(int index){
	if(index >= excitNeurGrpList.size())
		throw SpikeStreamException("Failed to get random neuron ID. Index out of range: " + QString::number(index));

	NeuronGroup* neurGrp = excitNeurGrpList.at(index);
	unsigned ranIndx = Util::getRandomUInt(0, neurGrp->size() - 1);
	return neurGrp->getNeuronMap()->values().at(ranIndx)->getID();
}


/*! Loads up the default parameters for the available synapse types. */
void ConnectionGroupBuilder::loadDefaultParameters(){
	defaultParameterMaps.clear();
	NetworkDao netDao(Globals::getNetworkDao()->getDBInfo());
	QList<SynapseType> synTypeList = netDao.getSynapseTypes();
	for(int i=0; i<synTypeList.size(); ++i)
		defaultParameterMaps[synTypeList.at(i).getID()] = netDao.getDefaultSynapseParameters(synTypeList.at(i).getID());
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

	QTextStream in(&delaysFile);
	QString line;

	//List tracking connection index at each node
	QList<int> conIndxList;
	for(int i=0; i<numberOfNodes; ++i)
		conIndxList.append(0);

	//Load delays from file in same order as for weights
	while (!in.atEnd()) {
		line = in.readLine();
		if(!line.isEmpty()){
			QStringList strList = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

			//Run some checks
			if(strList.size() != neurGrpConList.size())
				throw SpikeStreamException("Number of nodes in connection and delay matrix do not match");

			//Work through all the FROM indexes connecting TO the connection group represented by the row.
			for(int i=0; i<strList.size(); ++i){
				double tmpDelay = Util::getFloat(strList.at(i));
				if(tmpDelay != 0.0){
					//Have found a connection - is the index of this connection out of range?
					if(conIndxList[i] >= neurGrpConList[i].size())
						throw SpikeStreamException("Delay connection index: " + QString::number(conIndxList[i]) + " >= number of connections loaded from connection matrix: " + QString::number(neurGrpConList[i].size()) );

					//Delay should be greater than 0
					if(tmpDelay < 0.0)
						throw SpikeStreamException("Delay must be greater than or equal to zero: " + QString::number(tmpDelay));

					//Store delay
					neurGrpConList[i][conIndxList[i]].delay = tmpDelay / spikePropagationSpeed;

					//Increase the connection index for this node
					++conIndxList[i];
				}
			}
		}
	}

	//Check number of connections FROM each node is correct
	for(int nodeIdx = 0; nodeIdx < numberOfNodes; ++nodeIdx){
		if(conIndxList[nodeIdx] != neurGrpConList[nodeIdx].size())
			throw SpikeStreamException("Mismatch between number of delay connections " + QString::number(conIndxList[nodeIdx]) + " and number of connections loaded from connection matrix: " + QString::number(neurGrpConList[nodeIdx].size()) );
	}

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
			QStringList strList = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			if(strList.size() != numberOfNodes)
				throw SpikeStreamException("Number of nodes in connection matrix does not equal number of nodes loaded: Line: " + line + "; number of nodes: " + QString::number(numberOfNodes) + "; string list size: " + QString::number(strList.size()));

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
	calculateThresholds();
}


/*! Normalize weights so that the sum of all weights is equal to 1 * rewire probability */
void ConnectionGroupBuilder::normalizeWeights(){
	//Find the total weight
	double weightTotal = 0.0;
	for(int nodeIdx=0; nodeIdx<neurGrpConList.size(); ++nodeIdx){
		QList<ConnectionInfo>& conInfoList = neurGrpConList[nodeIdx];
		for(int conIdx=0; conIdx<conInfoList.size(); ++conIdx){
			weightTotal += conInfoList.at(conIdx).weight ;
		}
	}

	//Multiply weights so that the total = 1* rewire probability
	double weightFactor = rewireProbability / weightTotal;
	for(int nodeIdx=0; nodeIdx<neurGrpConList.size(); ++nodeIdx){
		QList<ConnectionInfo>& conInfoList = neurGrpConList[nodeIdx];
		for(int conIdx=0; conIdx<conInfoList.size(); ++conIdx){
			conInfoList[conIdx].weight *= weightFactor;
		}
	}
}


/*! Prints out the connections for debugging. */
void ConnectionGroupBuilder::printConnections(bool printToFile){
	QList< QList<ConnectionInfo> >& nodeConList = neurGrpConList;
	if(rewireToConnections)
		nodeConList = toNeurGrpConList;

	if(printToFile){
		QFile logFile(Globals::getSpikeStreamRoot() + "/log/ConnectionGroupBuilder.log");
		if(!logFile.open(QFile::WriteOnly | QFile::Truncate))
			throw SpikeStreamIOException("Cannot open log file for ConnectionGroupBuilder. Path: " + logFile.fileName());
		QTextStream logOut(&logFile);
		logOut<<"---------------------  CONNECTIONS -------------------"<<endl;
		for(int i=0; i<nodeConList.size(); ++i){
			logOut<<"Neuron: "<<i<<" Connections."<<endl;
			QList<ConnectionInfo>& conInfoList = nodeConList[i];
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
	for(int i=0; i<nodeConList.size(); ++i){
		cout<<"Neuron: "<<i<<" Connections."<<endl;
		QList<ConnectionInfo>& conInfoList = nodeConList[i];
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
	rewireProbability = rewireProb;

	minDelayRange = Util::getPositiveDoubleParameter("min_delay_range", parameterMap);
	maxDelayRange = Util::getPositiveDoubleParameter("max_delay_range", parameterMap);

	/* Get spike propagation speed. This is in m/s.
		Multiply by 1000 to get mm/s; divide by 1000 to get mm/ms. */
	spikePropagationSpeed = Util::getPositiveDoubleParameter("spike_propagation_speed", parameterMap);

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

	rewireToConnections = Util::getBoolParameter("rewire_to_connections", parameterMap);
}



