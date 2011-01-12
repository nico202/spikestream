#ifndef RASTERMODEL_H
#define RASTERMODEL_H

//SpikeStream includes
#include "NeuronGroup.h"

//Qwt includes
#include "qwt_raster_data.h"
#include "qwt_double_interval.h"

//Qt includes
#include <QHash>

//Other includes
#include <vector>
using namespace std;


namespace spikestream {

	/*! Class holding spiking data that is used to plot the raster. */
	class RasterModel : public QwtRasterData {
		public:
			RasterModel(QList<NeuronGroup*>& neuronGroupList);
			~RasterModel();
			void addData(const QList<unsigned>& firingNeuronIDs, unsigned timeStep);
			QwtRasterData *copy() const;
			int getMinX();
			int getMaxX();
			int getMinY();
			int getMaxY();
			QwtDoubleInterval range() const;
			double value(double x, double y) const;

		private:
			//====================  VARIABLES  ====================
			/*! List of neuron groups that are being monitored */
			QList<NeuronGroup*> neuronGroupList;

			/*! Array of vectors holding the firing neuron IDs at each time step */
			QHash<unsigned, bool>* dataArray;

			/*! Number of time steps within window */
			int numTimeSteps;

			/*! Total number of data points */
			int dataCount;

			/*! Data is updated continuously in a rotational manner.
				This index refers to the first point that is read from the data array */
			int readIndex;

			/*! Data is updated continuously in a rotational manner.
				This index refers to the location where the next incoming data is written to the data array */
			int writeIndex;

			/*! The time step value corresponding to the data index. */
			unsigned minTimeStep;

			/*! Maximum neuronID. Calcuated from neuron groups. */
			unsigned maxNeuronID;
	};

}

#endif//RASTERMODEL_H
