#ifndef MEMBRANEPOTENTIALGRAPHWIDGET_H
#define MEMBRANEPOTENTIALGRAPHWIDGET_H

//SpikeStream includes
#include "AbstractGraphWidget.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QWidget>

namespace spikestream {

	class MembranePotentialGraphWidget : public AbstractGraphWidget {
		Q_OBJECT

		public:
			MembranePotentialGraphWidget(QWidget* parent=0);
			~MembranePotentialGraphWidget();
			void addData(float membranePotential, timestep_t timeStep);
			virtual void paintLabels(QPainter& painter);
			virtual void paintData(QPainter& painter);


		private:
			//=====================  VARIABLES  ======================
			/*! Color of plot curve */
			QRgb plotColor;

			/*! Old X location to draw lines */
			int oldXPos;

			/*! Old Y location to draw lines */
			int oldYPos;

			int newXPos;

			int newYPos;


	};
}

#endif//MEMBRANEPOTENTIALGRAPHWIDGET_H
