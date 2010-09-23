#ifndef STANDARDSTDPFUNCTION_H
#define STANDARDSTDPFUNCTION_H

//SpikeStream includes
#include "AbstractSTDPFunction.h"

namespace spikestream {

	class StandardSTDPFunction : public AbstractSTDPFunction {
		public:
			StandardSTDPFunction();
			~StandardSTDPFunction();
			float* getPreArray();
			int getPreLength();
			float* getPostArray();
			int getPostLength();
			float getMinWeight();
			float getMaxWeight();

		private:
			//=====================  VARIABLES  ====================
			/*! Array of the pre values */
			float* preArray;

			/*! Array of the post values */
			float* postArray;

			/*! Size of the arrays - could be turned into a parameter if necessary */
			static const int ARRAY_LENGTH = 20;


			//=====================  METHODS  ======================
			void buildStandardSTDPFunction();
			void checkFunctionUpToDate();
			void print();
	};

}

#endif//STANDARDSTDPFUNCTION_H
