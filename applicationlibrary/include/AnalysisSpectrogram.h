#ifndef ANALYSISSPECTROGRAM_H
#define ANALYSISSPECTROGRAM_H

//SpikeStream includes
#include "AnalysisSpectrogram.h"

//Qt includes
#include <qprinter.h>
#include <qprintdialog.h>

//Qwt includes
#include "qwt_plot.h"
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_raster_data.h>


namespace spikestream {

	/*! Zoom class required by Qwt. */
	class MyZoomer: public QwtPlotZoomer {
		public:
			MyZoomer(QwtPlotCanvas *canvas): QwtPlotZoomer(canvas)	{
				setTrackerMode(AlwaysOn);
			}

			virtual QwtText trackerText(const QwtDoublePoint &pos) const {
				QColor bg(Qt::white);
				bg.setAlpha(200);
				QwtText text = QwtPlotZoomer::trackerText(pos);
				text.setBackgroundBrush( QBrush( bg ));
				return text;
			}
	};


	/*! Displays a spectrogram using the supplied data. Used for information spectrogram plots. */
	class AnalysisSpectrogram : public QwtPlot {
		Q_OBJECT

		public:
			AnalysisSpectrogram(QWidget *parent, QwtRasterData* data);
			~AnalysisSpectrogram();
			void showContour(bool on);
			void showSpectrogram(bool on);
			void printPlot();
			void rescale();

		private:
			QwtPlotSpectrogram *d_spectrogram;
	};

}


#endif//ANALYSISSPECTROGRAM_H


