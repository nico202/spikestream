#ifndef STDPFUNCTIONS_H
#define STDPFUNCTIONS_H

//SpikeStream includes
#include "AbstractSTDPFunction.h"
#include "ParameterInfo.h"

//Qt includes
#include <QHash>


//Other includes
#include <vector>

namespace spikestream {

	/*! Holds STDP functions for NeMo. */
	class STDPFunctions {

		public:
			static void cleanup();
			static QString getFunctionDescription(unsigned functionID);
			static QList<unsigned> getFunctionIDs();
			static QHash<QString, double> getParameters(unsigned functionID);
			static void setParameters(unsigned functionID, QHash<QString, double>& newParameterMap);
			static QHash<QString, double> getDefaultParameters(unsigned functionID);
			static QList<ParameterInfo> getParameterInfoList(unsigned functionID);
			static float* getPreArray(unsigned functionID);
			static int getPreLength(unsigned functionID);
			static float* getPostArray(unsigned functionID);
			static int getPostLength(unsigned functionID);
			static float getMinWeight(unsigned functionID);
			static float getMaxWeight(unsigned functionID);

		private:
			//=====================  VARIABLES  ====================
			/*! Type ID for the standard STDP function.
				NOTE: These should start at 0 and increase continuously. */
			static unsigned STANDARD_STDP;

			/*! Map containing instances of classes implementing each of the functions */
			static QHash<unsigned, AbstractSTDPFunction*> functionMap;

			/*! Records when function map has been filled */
			static bool initialized;

			//=====================  METHODS  ======================
			static void checkFunctionID(unsigned functionID);
			static void checkInitialization();
			static void initialize();
	};


}


#endif//STDPFUNCTIONS_H

