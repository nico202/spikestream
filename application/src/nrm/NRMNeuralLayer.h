#ifndef NRMNEURALLAYER_H
#define NRMNEURALLAYER_H

//SpikeStream includes
#include "NRMLayer.h"
#include "NRMConnection.h"
#include "NRMNeuron.h"

//Qt includes
#include <QList>

//Other includes

/*! Holds information about a NRM neural layer */
class NRMNeuralLayer : public NRMLayer {
	public:
		//=========================  METHODS  ============================
		NRMNeuralLayer();
		virtual ~NRMNeuralLayer();
		void addConnectionPath(NRMConnection* connection);
		void createConnections();
		void createNeurons();
		QList<NRMConnection*> getConnections();
		NRMConnection* getConnectionById(int id);
		NRMNeuron* getNeuron(unsigned int neurNum);
		unsigned int getNumberOfConnections();
		unsigned int getNumConnsPerNeuron();
		unsigned int getPatternArraySize();
		void print();
		void printConnections();
		void printNeuronConnections(unsigned int neurNum);
		void printTraining();

		//========================  VARIABLES  ===========================
		int generalisation;
		int spreading;
		int altParam1;
		int altParam2;
		int type; //FIXME: NOT SURE IF THIS IS NEEDED
		unsigned int trainingStringCount;

		/*! Neural type can be NEURAL_LAYER or MAGNUS_WIN_PREV */
		int neuralType;
		int inTrack;

		/*! Holds the connections associated with this layer.
			All connections are *TO* the input of this layer and they may be from:
				>> The state of this or another layer
				>> The previous state of this or another layer
				>> An input layer
		 */
		QList<NRMConnection*> connectionList;

		/*! Array of neurons */
		NRMNeuron** neuronArray;

};

#endif // NRMNEURALLAYER_H


