#ifndef SPIKERASTERWIDGET_H
#define SPIKERASTERWIDGET_H

//SpikeStream includes
#include "NeuronGroup.h"

//Qt includes
#include <QWidget>
#include <QVector>
#include <QImage>
#include <QTime>
#include <QPainter>

namespace spikestream {

	/*! Plots raster of firing data. */
	class SpikeRasterWidget : public QWidget {
		Q_OBJECT

		public :
			SpikeRasterWidget(QList<NeuronGroup*>& neuronGroupList, QWidget* parent=0);
			~SpikeRasterWidget();
			void addSpikes(const QList<unsigned>& firingNeuronIDs, int timeStep);
			void setBlackAndWhite(bool on);


		protected:
			void mouseDoubleClickEvent (QMouseEvent* event);
			void paintEvent(QPaintEvent* event);
			void resizeEvent(QResizeEvent* event);


		private:
			//=====================  VARIABLES  =====================
			/*! List of neuron groups that are being monitored.
				Filters spikes for ones within the particular neuron groups. */
			QList<NeuronGroup*> neuronGroupList;

			/*! Offset for spikes from each neuron group.
				Key is the neuron group ID; value is the offset. */
			QHash<unsigned, unsigned> neurGrpOffsetMap;

			/*! Color of spikes for each neuron group */
			QHash<unsigned, unsigned> neurGrpColorMap;

			/*! Image onto which spikes and buffer are painted */
			QImage* bufferImage;

			/*! Number of time steps on the horizontal axis */
			int numTimeSteps;

			/*! Number of neurons being monitored */
			int numNeurons;

			/*! Minimum time step on the X axis*/
			int minTimeStep;

			/*! Width of the widget */
			int widgetWidth;

			/*! Height of the widget */
			int widgetHeight;

			/*! Width of the image */
			int imageWidth;

			/*! Height of the image */
			int imageHeight;

			/*! Flag to indicate whether painter should redraw axes */
			bool updateAxes;

			/*! Height of X axis above botton */
			int xAxisPadding;

			/*! X axis tick length */
			int xAxisTickLength;

			/*! Indent of Y axis */
			int yAxisPadding;

			/*! Y axis tick length */
			int yAxisTickLength;

			/*! Size of the axes font in pixels */
			int fontSize;

			/*! Size of the neuron group names in pixels */
			int neurGrpNameFontSize;

			/*! Color of background */
			QRgb backgroundColor;

			/*! Color of axes */
			QRgb axesColor;

			/*! Controls whether spikes are rendered with varying hues or black and white. */
			bool blackAndWhiteMode;


			//========================  METHODS  =======================
			QString getFilePath(QString fileFilter);
			QList<unsigned> getHueList(unsigned numItems);
			void increaseTimeStep(int currentTimeStep);
			void paintNeuronGroupNames(QPainter& painter);
			void paintYAxis(QPainter&);
			void paintYAxisTick(QPainter& painter, int yPos, int label);
			void paintXAxis(QPainter&);
			void paintXAxisTick(QPainter&, int xval, int xlable);
			void paintAxes(QPainter&);

	};

}

#endif//SPIKERASTERWIDGET_H
