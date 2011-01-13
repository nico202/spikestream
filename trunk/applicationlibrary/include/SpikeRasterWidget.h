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


		protected:
			void paintEvent(QPaintEvent* event);
			void resizeEvent(QResizeEvent* event);


		private:
			//=====================  VARIABLES  =====================
			/*! List of neuron groups that are being monitored.
				Filters spikes for ones within the particular neuron groups. */
			QList<NeuronGroup*> neuronGroupList;

			QVector< QPair<int, int> > m_spikes;
			QTime m_runtime;
			int m_updates;

			/*! buffer each slice independently, only update the last one  */
			QVector<QImage> imageBufferVector;//! \todo deal with resizing here

			/*! Number of neurons */
			int numNeurons;

			qreal m_yscale;

			/*! Width of the buffer */
			int bufferWidth;

			/*! Number of time steps on the horizontal axis */
			int numTimeSteps; // for display, not for running

			//! \todo avoid overflow!
			/*! Last cycle for which we have complete data */
			int m_cycle;

			/*! Location in buffer vector to which new data should be added, corresponding to the
			 * interval m_fillCycle to m_fillCycle + m_temporalResolution */
			int writeIndex;

			/*! Starting point for reading from image buffer vector */
			int readIndex;

			/*! Records when read index should be incremented.
				This occurs when entire buffer has been filled.  */
			bool incrementReadIndex;

			/*! Minimum time step */
			int minTimeStep;

			/*! Width of the widget */
			int widgetWidth;

			/*! Height of the widget */
			int widgetHeight;

			/*! Height of X axis above botton */
			int xAxisPadding;

			/*! X axis tick length */
			int xAxisTickLength;

			/*! Indent of Y axis */
			int yAxisPadding;

			/*! Y axis tick length */
			int yAxisTickLength;

			/*! Size of the font in pixels */
			int fontSize; // in pixels

			int m_fillCycle;
		//	QPainter m_fillPainter;

			// Axis variables
			int m_axisPadding;
			int m_axisTickSize;


			//========================  METHODS  =======================
			void addSpike(int time, int neuron);
			void increaseTimeStep(int currentTimeStep);
			void paintData(QPainter&);
			void paintYAxis(QPainter&);
			void paintYAxisTick(QPainter& painter, int yPos, int label);
			void paintXAxis(QPainter&);
			void paintXAxisTick(QPainter&, int xval, int xlable);
			void paintAxes(QPainter&);

	};

}

#endif//SPIKERASTERWIDGET_H
