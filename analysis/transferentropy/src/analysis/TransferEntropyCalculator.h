#ifndef TRANSFERENTROPYCALCULATOR_H
#define TRANSFERENTROPYCALCULATOR_H

//SpikeStream includes
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>

//Other includes
#include <vector>
using namespace std;

namespace spikestream {

	class TransferEntropyCalculator {
		public:
			TransferEntropyCalculator(unsigned k_param, unsigned l_param, unsigned timeWindow);
			~TransferEntropyCalculator();
			QString getBitString(unsigned num);
			QHash<unsigned, double>& getI_k_probs() { return I_k_probs; }
			QHash<unsigned, double>& getI_k_plus_1_probs() { return I_k_plus_1_probs; }
			double getTransferEntropy(unsigned startTimeStep, vector<unsigned>& fromNeuronData, vector<unsigned>& toNeuronData);
			void load_I_k_probabilities(unsigned startTimeStep, vector<unsigned>& iVector);
			void load_I_k_plus_1_probabilities(unsigned startTimeStep, vector<unsigned>& iVector);
			void load_I_k_J_k_probabilities(unsigned startTimeStep, vector<unsigned>& jVector, vector<unsigned>& iVector);
			void load_I_k_plus_1_J_k_probabilities(unsigned startTimeStep, vector<unsigned>& jVector, vector<unsigned>& iSVector);

		private:
			//=============================  VARIABLES  ============================
			/*! k parameter of analysis */
			unsigned k_param;

			/*! l (L) parameter of analysis */
			unsigned l_param;

			/*! time window of analysis */
			unsigned timeWindow;

			/*! Map of probabilities of sequences of length k in I */
			QHash<unsigned, double> I_k_probs;

			/*! Map of probabilities of sequences of length k+1 in I */
			QHash<unsigned, double> I_k_plus_1_probs;

			/*! Map of combined probabilities of sequences of length k in I and sequences of length l in J */
			QHash<unsigned, double> I_k_J_l_probs;

			/*! Map of combined probabilities of sequences of length k+1 in I and sequences of length l in J */
			QHash<unsigned, double> I_k_plus_1_J_1_probs;


			//=============================  METHODS  ==============================
			void printMap(QHash<unsigned, double>& map, QString name);

	};

}

#endif//TRANSFERENTROPYCALCULATOR_H