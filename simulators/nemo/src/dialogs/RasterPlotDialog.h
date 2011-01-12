#ifndef RASTERPLOTDIALOG_H
#define RASTERPLOTDIALOG_H

//SpikeStream includes
#include "NeuronGroup.h"
#include "RasterModel.h"
#include "RasterView.h"

//Qt includes
#include <QDialog>

namespace spikestream {

	class RasterPlotDialog : public QDialog {
		Q_OBJECT

		public:
			RasterPlotDialog(QList<NeuronGroup*> neuronGroupList, QWidget* parent = 0);
			~RasterPlotDialog();
			void addData(const QList<unsigned>& firingNeuronIDs, unsigned timeStep);

		private:
			//====================  VARIABLES  ======================
			/*! The model holding the spiking data for each time step. */
			RasterModel* rasterModel;

			/*! View displaying the model */
			RasterView* rasterView;
	};

}

#endif//RASTERPLOTDIALOG_H
