#ifndef TESTTRANSFERENTROPYCALCULATOR_H
#define TESTTRANSFERENTROPYCALCULATOR_H

//SpikeStream includes
#include "TransferEntropyCalculator.h"
using namespace spikestream;

//Qt includes
#include <QtTest>
#include <QString>

namespace spikestream {

	class TestTransferEntropyCalculator : public QObject {
		Q_OBJECT

		private slots:
			void testPresentationData();
			void testZafeiriosCode();
		private:
			void testGetBitString();
			void testLoad_I_k_probabilities();
			void testLoad_I_k_plus_1_probabilities();
			void testLoad_I_k_J_k_probabilities();
			void testLoad_I_k_plus_1_J_l_probabilities();
			void testGetTransferEntropy();

			inline double round(double n, unsigned d){
				  return floor(n * pow(10, d) + 0.5) / pow(10, d);
			}

	};

}

#endif//TESTTRANSFERENTROPYCALCULATOR_H


