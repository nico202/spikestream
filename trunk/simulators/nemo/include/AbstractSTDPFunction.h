#ifndef ABSTRACTSTDPFUNCTION_H
#define ABSTRACTSTDPFUNCTION_H

//SpikeStream includes
#include "ParameterInfo.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>

namespace spikestream {

	/*! Abstract interface for an STDP function */
	class AbstractSTDPFunction {
		public:
			AbstractSTDPFunction();
			virtual ~AbstractSTDPFunction();
			virtual timestep_t getApplySTDPInterval();
			virtual QHash<QString, double> getParameters();
			virtual void setParameters(QHash<QString, double>& newParameterMap);
			virtual QHash<QString, double> getDefaultParameters();
			virtual QList<ParameterInfo> getParameterInfoList();
			virtual float* getPreArray() = 0;
			virtual int getPreLength() = 0;
			virtual float* getPostArray() = 0;
			virtual int getPostLength() = 0;
			virtual float getMinWeight() = 0;
			virtual float getMaxWeight() = 0;
			virtual float getReward();
			virtual void print() = 0;

		protected:
			//======================  VARIABLES  ========================
			/*! List defining the parameters */
			QList<ParameterInfo> parameterInfoList;

			/*! Map containing default values of the parameters */
			QHash<QString, double> defaultParameterMap;

			/*! Map containing the current values of the parameters */
			QHash<QString, double> parameterMap;

			/*! Records if the function is up to date.
				This value is set to false when the parameters are changed */
			bool functionUpToDate;


			//=====================  METHODS  =======================
			double getParameter(QString parameterName);
	};

}

#endif//ABSTRACTSTDPFUNCTION_H
