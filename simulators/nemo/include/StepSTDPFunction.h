#ifndef STEPSTDPFUNCTION_H
#define STEPSTDPFUNCTION_H

//SpikeStream includes
#include "AbstractSTDPFunction.h"

namespace spikestream {

	class StepSTDPFunction : public AbstractSTDPFunction {
		public:
			StepSTDPFunction();
			~StepSTDPFunction();
			float* getPreArray();
			int getPreLength();
			float* getPostArray();
			int getPostLength();
			float getMinWeight();
			float getMaxWeight();
			void print();

		private:
			//=====================  VARIABLES  ====================
			/*! Array of the pre values */
			float* preArray;

			/*! Array of the post values */
			float* postArray;

			/*! Length of pre array */
			int preLength;

			/*! Length of post array */
			int postLength;


			//=====================  METHODS  ======================
			void buildStepSTDPFunction();
			void checkFunctionUpToDate();
			void cleanUp();
	};

}

#endif//STEPSTDPFUNCTION_H
