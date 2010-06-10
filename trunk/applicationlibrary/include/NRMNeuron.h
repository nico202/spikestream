#ifndef NRMNEURON_H
#define NRMNEURON_H

//Qt includes
#include <QList>

namespace spikestream {

	class NRMNeuron {
		public:
			NRMNeuron();
			~NRMNeuron();

			void addTraining(unsigned char* inBitArray, unsigned int inBitArrLen, unsigned char output);
			QList<unsigned char*> getTraining();
			unsigned int getTrainingArrayLength() { return trainingArrayLength; }
			void printTraining();


		private:
			//=============================  VARIABLES  ==================================
			/*! Training of the neuron.
					The zero byte in each array is the output.
					The rest is a series of bits encoding the output from other neurons */
			QList<unsigned char*> trainingList;

			/*! Length of the byte array in the training list. */
			unsigned int trainingArrayLength;

	};
}

#endif // NRMNEURON_H
