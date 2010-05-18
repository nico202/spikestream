#ifndef NETWORKDAO_H
#define NETWORKDAO_H

//SpikeStream includes
#include "AbstractDao.h"
#include "Box.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "NetworkInfo.h"
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"
#include "NeuronType.h"
#include "ParameterInfo.h"
#include "SynapseType.h"
#include "WeightlessNeuron.h"
using namespace spikestream;

//Qt includes
#include <QString>

namespace spikestream{

	/*! Abstraction layer on top of the SpikeStreamNetwork database.
		Used for light operations taking place in the same thread as the calling
		function. Operations that are more database intensive can be found in
		the NetworkDaoThread class. */
	class NetworkDao : public AbstractDao {
		public:
			NetworkDao(const DBInfo& dbInfo);
			virtual ~NetworkDao();

			void addNetwork(NetworkInfo& netInfo);
			void addWeightlessConnection(unsigned int connectionID, unsigned int patternIndex);
			unsigned int addWeightlessNeuronTrainingPattern(unsigned int neuronID, const unsigned char* patternArray, bool output, unsigned int patternArraySize);
			void deleteNetwork(unsigned int networkID);
			void deleteAllNetworks();
			void getAllFromConnections(unsigned int networkID, QHash<unsigned int, QHash<unsigned int, bool> >& connMap);
			void getAllToConnections(unsigned int networkID, QHash<unsigned int, QHash<unsigned int, bool> >& connMap);
			QList<unsigned int> getFromConnections(unsigned int fromNeuronID);
			QList<unsigned int> getToConnections(unsigned int toNeuronID);
			QList<Connection> getConnections(unsigned int fromNeuronID, unsigned int toNeuronID);
			unsigned int getConnectionGroupSize(unsigned int connGrpID);
			QList<ConnectionGroupInfo> getConnectionGroupsInfo(unsigned int networkID);
			QList<Connection*> getConnections(unsigned int connectionMode, unsigned int singleNeuronID, unsigned int toNeuronID);
			QList<NetworkInfo> getNetworksInfo();
			unsigned int getNeuronCount(unsigned int networkID, const Box& box);
			Box getNeuronGroupBoundingBox(unsigned int neurGrpID);
			unsigned int getNeuronGroupID(unsigned int neuronID);
			QList<NeuronGroupInfo> getNeuronGroupsInfo(unsigned int networkID);
			QList<unsigned int> getNeuronIDs(unsigned int networkID);
			NeuronType getNeuronType(unsigned int neuronTypeID);
			QList<NeuronType> getNeuronTypes();
			QHash<QString, double> getNeuronParameters(const NeuronGroupInfo& neurGrpInfo);
			QList<SynapseType> getSynapseTypes();
			WeightlessNeuron* getWeightlessNeuron(unsigned int neuronID);
			bool isWeightlessNetwork(unsigned int networkID);
			bool isWeightlessNeuron(unsigned int neuronID);
			void setNeuronParameters(QHash<QString, double>& paramMap);
			void setTempWeight(unsigned int fromNeurID, unsigned int toNeurID, double tempWeight);

		private:
			//=========================  METHODS  ===========================
			QList<ParameterInfo> getNeuronParameterInfo(const NeuronType& neuronType);

	};

}

#endif//NETWORKDAO_H


