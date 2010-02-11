#ifndef HEATCOLORBAR_H
#define HEATCOLORBAR_H

//Qt includes
#include <QColor>
#include <QPaintEvent>
#include <QSize>
#include <QWidget>

namespace spikestream {

	/*! Displays a bar of colors corresponding to different temperatures */
	class HeatColorBar : public QWidget {
		Q_OBJECT

		public:
			HeatColorBar(QWidget* parent);
			~HeatColorBar();
			double getMaxValue() { return maxValue; }
			void setMaxValue(double maxValue);

		protected:
			void paintEvent(QPaintEvent* event);


		private:
			//====================  VARIABLES  =====================
			//Colours of the heat bars
			QColor color0;
			QColor color1;
			QColor color2;
			QColor color3;
			QColor color4;
			QColor color5;
			QColor color6;
			QColor color7;
			QColor color8;
			QColor color9;
			QColor color10;

			//Colours of the fonts
			QColor fontColor;

			/*! The maximum value of the colour bar.
				The range will be divided  between this value and zero using a linear scale. */
			double maxValue;
	};

}

#endif//HEATCOLORBAR_H

