#ifndef WEIGHTLESSNEURON_H
#define WEIGHTLESSNEURON_H

//Qt includes
#include <QHash>

namespace spikestream {

    /*! Handier to use byte than unsigned char */
    typedef unsigned char byte;

    class WeightlessNeuron {
	public:
	    WeightlessNeuron(QHash<unsigned int, QList<unsigned int> >& connectionMap, unsigned int id);
	    ~WeightlessNeuron();
	    void addTraining(QByteArray& newData, unsigned int output);
	    QHash<unsigned int, QList<unsigned int> > getConnectionMap() { return connectionMap; }
	    int getNumberOfConnections() { return numberOfConnections; }
	    double getFiringStateProbability(byte inPatArr[], int inPatArrLen, int firingState);
	    unsigned int getHammingThreshold() { return hammingThreshold; }
	    QList<byte*> getTrainingData() { return trainingData; }
	    int getTrainingDataLength() { return trainingDataLength; }
	    double getTransitionProbability(const QList<unsigned int>& neurIDList, const QString& x0Pattern, int firingState);
	    void setGeneralization(double generalization);

	private:
	    //===================  VARIABLES  ===================
	    /*! Id of the neuron */
	    unsigned int id;

	    /*! Map linking each position in the pattern string with
		a neuron id. Key is the connected neuron id; value is the index in the pattern string. */
	    QHash<unsigned int, QList<unsigned int> > connectionMap;

	    /*! Number of connections to this weightless neuron. This has to be calculated from the connection map */
	    int numberOfConnections;

	    /*! List of training arrays */
	    QList<byte*> trainingData;

	    /*! Length of each array of training data */
	    int trainingDataLength;

	    /*! The minimum Hamming distance for a match between two patterns */
	    unsigned int hammingThreshold;

	    //====================  METHODS  =====================
	    void buildInputPattern(byte inPatArr[], int inPatArrSize, bool selArr[], int selArrSize, QHash<unsigned int, byte>& firingNeuronIndexMap);
	    void printConnectionMap();
	    void printSelectionArray(bool selArr[], int arrSize);
	    void printTraining();
    };

}

#endif//WEIGHTLESSNEURON_H

