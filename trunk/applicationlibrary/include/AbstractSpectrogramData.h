#ifndef ABSTRACTSPECTROGRAMDATA_H
#define ABSTRACTSPECTROGRAMDATA_H

//SpikeStream includes
#include "AnalysisInfo.h"
using namespace spikestream;

//Qwt includes
#include "qwt_raster_data.h"

//Qt includes
#include <QHash>

namespace spikestream {

	/*! Abstract spectrogram class that can be used to display information spectrograms for different types of analysis. */
	class AbstractSpectrogramData : public QwtRasterData {
		public:
			AbstractSpectrogramData(const AnalysisInfo& analysisInfo);
			virtual ~AbstractSpectrogramData();
			virtual QwtRasterData *copy() const = 0;
			virtual QwtDoubleInterval range() const;
			virtual void showAllTimeSteps(bool show);
			virtual void showSelectedTimeStep(unsigned int timeStep);
			virtual void setShowNeuronIDs(bool show);
			virtual double value(double x, double y) const;

		protected:
			//=======================  VARIABLES  =========================
			/*! Information about the analysis */
			AnalysisInfo analysisInfo;

			/*! Holds the processed data from the analysis.
				Key1 is the time step, key2 is the first neuron id key3 is the second neuron id
				data is the integration between the neurons */
			QHash<unsigned int, QHash<unsigned int, QHash<unsigned int, double> > > timeStepDataMap;

			/*! Minimum neuron id to set the bounding rectangle */
			unsigned int minNeuronID;

			/*! Maximum neuron id to set the bounding rectangle */
			unsigned int maxNeuronID;

			/*! Current time step that is being displayed */
			int selectedTimeStep;

			/*! Average over all time steps is shown */
			bool allTimeSteps;

			/*! Key pointing to unused time step value */
			unsigned int allTimeStepsKey;

			/*! Neuron IDs are displayed instead of numbers from 1 */
			bool showNeuronIDs;


			//=======================  METHODS  ===========================
			void addAllTimeStepsMap(const QList<unsigned int>& neurIDList);
			void addTimeStep(unsigned int timeStep, const QList<unsigned int>& neurIDList);
			virtual void loadData() = 0;
			virtual void printData();
			virtual void reset();
	};

}


#endif//ABSTRACTSPECTROGRAMDATA_H

