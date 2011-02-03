#ifndef RASTERVIEW_H
#define RASTERVIEW_H

//SpikeStream includes
#include "RasterModel.h"

//Qwt includes
#include "qwt_plot.h"
#include "qwt_plot_spectrogram.h"


namespace spikestream {

	/*! Displays a spike raster using the supplied data.*/
	class RasterView : public QwtPlot {
		Q_OBJECT

		public:
			RasterView(RasterModel* rasterModel, QWidget *parent);
			~RasterView();
			void printPlot();
			void rescale();

		private:
			//==================  VARIABLES  ====================
			/*! Spectrogram showing the spikes. */
			QwtPlotSpectrogram* qwtSpectrogram;

			/*! Model containing the data */
			RasterModel* rasterModel;
	};

}


#endif//RASTERVIEW_H


