#ifndef STDPFUNCTIONS_H
#define STDPFUNCTIONS_H

//Qt includes
#include <QHash>

//Other includes
#include <vector>

namespace spikestream {

	/*! Holds STDP functions for NeMo. */
	class STDPFunctions {

		public:
			static QString getFunctionDescription(unsigned functionID);
			static QList<unsigned> getFunctionIDs();
			static float* getPre(unsigned functionID);
			static int getPreLength(unsigned functionID);
			static float* getPost(unsigned functionID);
			static int getPostLength(unsigned functionID);
			static float getMinWeight(unsigned functionID);
			static float getMaxWeight(unsigned functionID);

		private:
			//=====================  VARIABLES  ====================
			/*! Type ID for the standard STDP function.
				NOTE: These should start at zero and increase continuously. */
			static unsigned STANDARD_STDP;

			/*! Map linking IDs with pre arrays */
			static QHash<unsigned, float* > preArrayMap;

			/*! Map linking IDs with the length of the pre arrays. */
			static QHash<unsigned, int> preArrayLengthMap;

			/*! Map linking IDs with post arrays */
			static QHash<unsigned, float* > postArrayMap;

			/*! Map linking IDs with the lenghts of the post arrays. */
			static QHash<unsigned, int> postArrayLengthMap;

			/*! Map linking IDs with min weights */
			static QHash<unsigned, float> minWeightMap;

			/*! Map linking IDs with max weights */
			static QHash<unsigned, float> maxWeightMap;

			/*! Records if the functions have been built. This is done on the first request. */
			static bool initialized;


			//===================  METHODS  =====================
			static void buildStandardStdpFunction();
			static void checkInitialization();
			static void initialize();
	};


}


#endif//STDPFUNCTIONS_H

