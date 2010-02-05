#ifndef WEIGHTLESSNEURON_H
#define WEIGHTLESSNEURON_H

//Qt includes
#include <QHash>

namespace spikestream {

	class WeightlessNeuron {
	public:
		WeightlessNeuron(QHash<unsigned int, QList<unsigned int> >& connectionMap, unsigned int id);
		~WeightlessNeuron();
		void addTraining(QByteArray& newData, unsigned int output);
		QHash<unsigned int, QList<unsigned int> >& getConnectionMap() { return connectionMap; }
		unsigned int getID() { return id; }
		int getNumberOfConnections() { return numberOfConnections; }
		double getFiringStateProbability(unsigned char inPatArr[], int inPatArrLen, int firingState);
		unsigned int getHammingThreshold() { return hammingThreshold; }
		QList<unsigned char*> getTrainingData() { return trainingData; }
		int getTrainingDataLength() { return trainingDataLength; }
		double getTransitionProbability(const QList<unsigned int>& neurIDList, const QString& x0Pattern, int firingState);
		void setGeneralization(double generalization);

	private:
		//===================  VARIABLES  ===================
		/*! Id of the neuron */
		unsigned int id;

		/*! Map linking each position in the pattern string with
			a neuron id. Key is the connected neuron id; value is a list containing indexs in the pattern string.
			This is a list because there can be several connections between the same neurons. */
		QHash<unsigned int, QList<unsigned int> > connectionMap;

		/*! Number of connections to this weightless neuron. This has to be calculated from the connection map */
		int numberOfConnections;

		/*! List of training arrays */
		QList<unsigned char*> trainingData;

		/*! Length of each array of training data */
		int trainingDataLength;

		/*! The minimum Hamming distance for a match between two patterns */
		unsigned int hammingThreshold;

		//====================  METHODS  =====================
		void buildInputPattern(unsigned char inPatArr[], int inPatArrSize, bool selArr[], int selArrSize, QHash<unsigned int, unsigned char>& firingNeuronIndexMap);
		void printConnectionMap();
		void printSelectionArray(bool selArr[], int arrSize);
		void printTraining();
	};

}

#endif//WEIGHTLESSNEURON_H

