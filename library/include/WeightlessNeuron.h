#ifndef WEIGHTLESSNEURON_H
#define WEIGHTLESSNEURON_H

//Qt includes
#include <QHash>

namespace spikestream {

    /*! Handier to use byte than unsigned char */
    typedef unsigned char byte;

    class WeightlessNeuron {
	public:
	    WeightlessNeuron(QHash<unsigned int, unsigned int>& connectionMap);
	    ~WeightlessNeuron();
	    void addTraining(QByteArray& newData, unsigned int output);
	    double getFiringStateProbability(byte inPatArr[], int inPatArrLen, byte firingState);
	    QList<byte*> getTrainingData() { return trainingData; }
	    int getTrainingDataLength() { return trainingDataLength; }
	    double getTransitionProbability(const QList<unsigned int>& neurIDList, const QString& x0Pattern, bool firingState);
	    void setGeneralization(double generalization);

	private:
	    //===================  VARIABLES  ===================
	    /*! Map linking each position in the pattern string with
		a neuron id. Key is the connected neuron id; value is the index in the pattern string. */
	    QHash<unsigned int, unsigned int> connectionMap;

	    /*! List of training arrays */
	    QList<byte*> trainingData;

	    /*! Length of each array of training data */
	    int trainingDataLength;

	    /*! The minimum Hamming distance for a match between two patterns */
	    unsigned int hammingThreshold;

	    //====================  METHODS  =====================
	    void buildInputPattern(byte inPatArr[], int inPatArrSize, bool selArr[], int selArrSize, QHash<unsigned int, byte>& firingNeuronIndexMap);
    };

}

#endif//WEIGHTLESSNEURON_H

