//SpikeStream includes
#include "ConnectionGroup.h"
#include "GlobalVariables.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

//Outputs debugging information about memory
//#define MEMORY_DEBUG

//Initialize static variables
unsigned ConnectionGroup::connectionIDCounter = LAST_CONNECTION_ID + 1;


/*! Empty constructor */
ConnectionGroup::ConnectionGroup(){
	#ifdef MEMORY_DEBUG
		cout<<"New connection group (empty constructor) with size: "<<sizeof(*this)<<endl;
	#endif//MEMORY_DEBUG
	connectionDeque = new deque<Connection>();
}


/*! Standard constructor */
ConnectionGroup::ConnectionGroup(const ConnectionGroupInfo& connGrpInfo){
    this->info = connGrpInfo;
	connectionDeque = new deque<Connection>();

	#ifdef MEMORY_DEBUG
		cout<<"New connection group (standard constructor) with size: "<<sizeof(*this)<<endl;
	#endif//MEMORY_DEBUG
}


/*! Destructor */
ConnectionGroup::~ConnectionGroup(){
	#ifdef MEMORY_DEBUG
		cout<<"Connection group destructor size of class: "<<sizeof(*this)<<"; size of map: "<<sizeof(*connectionDeque)<<"; number of connections: "<<connectionDeque->size()<<endl;
	#endif//MEMORY_DEBUG

	//Deletes connection map and all its dynamically allocated objects
	if(connectionDeque != NULL){
		clearConnections();
		delete connectionDeque;
	}
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Adds a connection to the group using the specified ID and returns the index of the connection.
	The connection can be accessed later using []. */
unsigned ConnectionGroup::addConnection(unsigned id, unsigned fromNeuronID, unsigned toNeuronID, float delay, float weight){
	//Store connection
	connectionDeque->push_back(Connection(id, fromNeuronID, toNeuronID, delay, weight));

	//Return index of connection
	return connectionDeque->size() - 1;
}


/*! Adds a connection to the group using a temporary ID and returns the index of the connection.
	The connection can be accessed later using []. */
unsigned ConnectionGroup::addConnection(unsigned int fromNeuronID, unsigned int toNeuronID, float delay, float weight){
	unsigned tmpID = getTemporaryID();

	//Store connection and return index
	connectionDeque->push_back(Connection(tmpID, fromNeuronID, toNeuronID, delay, weight));
	return connectionDeque->size() - 1;
}


/*! Returns iterator pointing to beginning of connection group */
ConnectionIterator ConnectionGroup::begin(){
	return connectionDeque->begin();
}


/*! Returns iterator pointing to end of connection group */
ConnectionIterator ConnectionGroup::end(){
	return connectionDeque->end();
}


/*! Removes all connections from this group */
void ConnectionGroup::clearConnections(){
	connectionDeque->clear();
}


/*! Returns the value of a named parameter.
	Throws an exception if the parameter cannot be found. */
double ConnectionGroup::getParameter(const QString& paramName){
	if(!parameterMap.contains(paramName))
		throw SpikeStreamException("Cannot find parameter with key: " + paramName + " in connection group with ID " + QString::number(info.getID()));
	return  parameterMap[paramName];
}


/*! Returns the ID of the synapse type of this connection group */
unsigned ConnectionGroup::getSynapseTypeID(){
	return info.getSynapseTypeID();
}


/*! Returns a reference to the operator at a specific index. */
Connection& ConnectionGroup::operator[] (unsigned index){
	if(index >= connectionDeque->size())
		throw SpikeStreamException("Connection vector index out of range: " + QString::number(index));
	return (*connectionDeque)[index];
}


/*! Returns true if the parameters have been set. */
bool ConnectionGroup::parametersSet(){
	if(getInfo().getSynapseType().getParameterCount() == parameterMap.size())
		return true;
	return false;
}


/*! Prints out the neuron group for debugging. */
void ConnectionGroup::print(bool printConnections){
	cout<<"------------- Connection Group: id="<<info.getID()<<"; '"<<info.getDescription().toStdString()<<"' ";
	cout<<"From: "<<info.getFromNeuronGroupID()<<" To: "<<info.getToNeuronGroupID()<<" ";
	cout<<info.getSynapseType().getDescription().toStdString()<<" -------------"<<endl;
	if(printConnections){
		ConnectionIterator endConGrp = this->end();
		for(ConnectionIterator conIter = this->begin(); conIter != endConGrp; ++conIter){
			conIter->print();
		}
	}
}


/*! Sets the description of the connection group */
void ConnectionGroup::setDescription(const QString& description){
	info.setDescription(description);
}


/*! Sets the FROM neuron group ID */
void ConnectionGroup::setFromNeuronGroupID(unsigned id){
	info.setFromNeuronGroupID(id);
}


/*! Sets the parameters of the connection group.
	Throws an exception if the parameters do not match those in the synapse type. */
void ConnectionGroup::setParameters(QHash<QString, double> &paramMap){
	//Get the neuron type associated with this neuron group
	SynapseType synType = getInfo().getSynapseType();
	QList<ParameterInfo> paramInfoList = synType.getParameterInfoList();
	if(paramInfoList.size() != paramMap.size())
		throw SpikeStreamException("ConnectionGroup: failed to set parameters. Mismatch between number of parameters.");
	foreach(ParameterInfo paramInfo, paramInfoList){
		if(!paramMap.contains(paramInfo.getName()))
			throw SpikeStreamException("ConnectionGroup: failed to set parameters. Missing parameter: " + paramInfo.getName());
	}

	//Parameters are ok - store map.
	this->parameterMap = paramMap;
}


/*! Sets the TO neuron group ID */
void ConnectionGroup::setToNeuronGroupID(unsigned id){
	info.setToNeuronGroupID(id);
}


/*--------------------------------------------------------*/
/*-------             PRIVATE METHODS              -------*/
/*--------------------------------------------------------*/

/*! Returns a temporary ID for adding connections */
unsigned ConnectionGroup::getTemporaryID(){
	return ++connectionIDCounter;
}

