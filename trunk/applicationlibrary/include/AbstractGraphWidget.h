#ifndef ABSTRACTGRAPHWIDGET_H
#define ABSTRACTGRAPHWIDGET_H

//Qt includes
#include <QWidget>
#include <QVector>
#include <QImage>
#include <QTime>
#include <QPainter>

namespace spikestream {

	/*! Plots raster of firing data. */
	class AbstractGraphWidget : public QWidget {
		Q_OBJECT

		public :
			AbstractGraphWidget(QWidget* parent=0);
			virtual ~AbstractGraphWidget();
			int getMaxY() { return maxY; }
			int getMinY() { return minY; }
			void setYRange(int minY, int maxY);

		protected:
			//=====================  VARIABLES  =====================
			/*! Image onto which spikes and buffer are painted */
			QImage* bufferImage;

			/*! Number of time steps on the horizontal axis */
			int numTimeSteps;

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

			/*! X axis tick length */
			int xAxisTickLength;

			/*! Indent of X axis when minY is small or > 0 */
			int xAxisPadding;

			/*! Indent of Y axis */
			int yAxisPadding;

			/*! Y axis tick length */
			int yAxisTickLength;

			/*! Size of the axes font in pixels */
			int axesLabelFontSize;

			/*! Size of the label names in pixels */
			int labelFontSize;

			/*! Color of background */
			QRgb backgroundColor;

			/*! Color of axes */
			QRgb axesColor;

			/*! Color of labels */
			QRgb labelsColor;

			//=========================  METHODS  ======================
			void buildBufferImage();
			void increaseTimeStep(int currentTimeStep);
			void mouseDoubleClickEvent (QMouseEvent* event);
			void paintEvent(QPaintEvent* event);
			void resizeEvent(QResizeEvent* event);
			virtual void paintAxes(QPainter&);
			virtual void paintData(QPainter&) = 0;
			virtual void paintLabels(QPainter&) = 0;


		private:
			//========================  VARIABLES  =====================
			/*! Minimum value of Y */
			int minY;

			/*! Maximum value of Y */
			int maxY;


			//========================  METHODS  =======================
			QString getFilePath(QString fileFilter);
			void paintYAxis(QPainter&);
			void paintYAxisTick(QPainter& painter, int yPos, int label);
			void paintXAxis(QPainter&);
			void paintXAxisTick(QPainter&, int xval, int xlable);


	};

}

#endif//ABSTRACTGRAPHWIDGET_H
