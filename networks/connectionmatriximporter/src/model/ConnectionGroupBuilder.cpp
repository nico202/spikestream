#include "ConnectionGroupBuilder.h"
#include "Util.h"
using namespace spikestream;


/*! Constructor */
ConnectionGroupBuilder::ConnectionGroupBuilder(QList<QString> nodeNamesList, Network* network){
	this->nodeNamesList = nodeNamesList;
	numberOfNodes = nodeNamesList.size();

	//Build map linking index in file with neuron groups


	//Store synapse type to be used on all connections
	synapseType = networkDao.getSynapseType("Izhikevich Synapse");
}


/*! Destructor */
ConnectionGroupBuilder::~ConnectionGroupBuilder(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

void ConnectionGroupBuilder::addConnectionGroups(Network* network, const QString& weightsFilePath, const QString& delaysFilePath, QHash<QString, double> parameterMap){
	//Load up the connections
	loadWeights(weightsFilePath);

	//Load up the delays
	loadDelays(delaysFilePath);

	//Work through all of the neuron groups
	for(QHash<unsigned, QList<Connection*> >::iterator iter = neurGrpConMap.begin(); iter != neurGrpConMap.end(); ++iter){
		addConnections(network, neurGrpIdxMap[iter.key()], iter.value(), parameterMap);
	}
}





/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

void ConnectionGroupBuilder::addConnections(Network* network, NeuronGroup* fromNeurGrp, QList<Connection*>& connectionsList, QHash<QString, double>& parameterMap){
	//Run some checks
	if(connectionsList.isEmpty())
		throw SpikeStreamException("Connections list must have at least one entry.");

	//Store parameters


	//Create new connection groups for connectections between from neuron group and external groups
	QList<ConnectionGroup*> newConGrpList;
	foreach(Connection* tmpCon, connectionsList){
		NeuronGroup* fromNeurGrp = neurGrpIndxMap[tmpCon->getFromNeuronID()];
		NeuronGroup* toNeurGrp = neurGrpIndxMap[tmpCon->getToNeuronID()];
		QString description = fromNeurGrp->getInfo().getName() + "->" + toNeurGrp->getInfo().getName();
		newConGrpList.append( new ConnectionGroup( ConnectionGroupInfo(0, description, fromNeurGrp->getID(), toNeurGrp->getID(), parameterMap, synapseType) ) );
	}

	//Create connection group for the connections within the from neuron group
	description = fromNeurGrp->getInfo().getName() + "->" + fromNeurGrp->getInfo().getName();
	ConnectionGroup* selfConGrp = new ConnectionGroup( ConnectionGroupInfo(0, description, fromNeurGrp->getID(), fromNeurGrp->getID(), parameterMap, synapseType) );

	//Create the connections
	for(NeuronMap::iterator fromIter = fromNeurGrp->begin(); fromIter != fromNeurGrp->end() && !stopThread; ++fromIter){
		for(NeuronMap::iterator toIter = fromNeurGrp->begin(); toIter != fromNeurGrp->end() && !stopThread; ++toIter){
			//Make connection within the from group
			if(rand() > rewireThreshold){
				selfConGrp->addConnection(fromIter.key(), toIter.key(), Util::getRandomUInt(minIntraDelay, maxIntraDelay), Util::getRandomDouble(minIntraWeight, maxIntraWeight));
			}
			//Make connection to another group
			else{
				//Select group at random
				int ranIndx = Util::getRandom(0, connectionsList.size());
				ConnectionGroup* tmpToConGrp = newConGrpList.at(ranIndx);
				Connection* tmpCon = connectionsList.at(ranIndx);

				//Add connection to random neuron in selected to group, avoiding duplicate connections.
				tmpConGrp->addConnection(fromIter.key(), getRandomNeuronID(tmpToConGrp, tmpConMap), tmpCon->delay, tmpCon->weight);
			}
		}
	}

	//Eliminate any connection groups without connections
	for(int i=0; i<newConGrpList.size(); ++i){
		if(newConGrpList.at(i)->size() == 0){
			newConGrpList.removeAt(i);
			--i;
		}
	}

	//Add connection groups to network
	newConGrpList.append(selfConGrp);
	network->addConnectionGroups(newConGrpList);
}


/*! Loads the weights up from the connection matrix file */
void ConnectionGroupBuilder::loadWeights(const QString& weightsFilePath){
	QFile weightsFile(weightsFilePath);
	neurGrpConMap.clear();

	//Check file exists and open it
	if(!weightsFile.exists())
		throw SpikeStreamIOException("Cannot find connection matrix file.");
	if (!weightsFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open connection matrix file for reading: " + weightsFile.fileName());

	//Load coordinates from file
	unsigned rowCntr = 0;
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
				if(tmpWeight != 0.0f){
					neurGrpConMap[i].append(Connection(i, rowCntr, 0, tmpWeight));
				}
			}
			++rowCntr;
		}
	}

	//Check row count is correct
	if(rowCntr != numberOfNodes)
		throw SpikeStreamException("Row count does not match number of nodes: " + QString::number(rowCntr));

	weightsFile.close();
}
