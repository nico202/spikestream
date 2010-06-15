//SpikeStream includes
#include "HeatColorBar.h"
using namespace spikestream;

//SpikeStream includes
#include "GlobalVariables.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QPainter>

//Other includes
#include "math.h"


/*! Constructor */
HeatColorBar::HeatColorBar(QWidget* parent) : QWidget(parent){
	//Set the size of the color bar
	setMinimumSize(400, 20);
	setMaximumSize(400, 20);

	//Set the colors
	color0.setRgbF(HEAT_COLOR_0);
	color1.setRgbF(HEAT_COLOR_1);
	color2.setRgbF(HEAT_COLOR_2);
	color3.setRgbF(HEAT_COLOR_3);
	color4.setRgbF(HEAT_COLOR_4);
	color5.setRgbF(HEAT_COLOR_5);
	color6.setRgbF(HEAT_COLOR_6);
	color7.setRgbF(HEAT_COLOR_7);
	color8.setRgbF(HEAT_COLOR_8);
	color9.setRgbF(HEAT_COLOR_9);
	color10.setRgbF(HEAT_COLOR_10);

	//Set up font colours
	fontColor.setRgb(0,255,0);

	//Default max value of the bar
	maxValue = DEFAULT_MAX_HEAT_COLOR_VALUE;
}


/*! Destructor */
HeatColorBar::~HeatColorBar(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the maximum value of the heat color bar. */
void HeatColorBar::setMaxValue(double maxValue){
	this->maxValue = maxValue;
	update();
}


/*----------------------------------------------------------*/
/*------               PROTECTED METHODS              ------*/
/*----------------------------------------------------------*/

/*! Overridden paint method inherited from QWidget */
void HeatColorBar::paintEvent(QPaintEvent*){
	QPainter painter(this);
	int areaWidth = width()/11;
	int areaHeight = height();

	//Paint the colour bars
	painter.fillRect(areaWidth * 0, 0, areaWidth, areaHeight, color0);
	painter.fillRect(areaWidth * 1, 0, areaWidth, areaHeight, color1);
	painter.fillRect(areaWidth * 2, 0, areaWidth, areaHeight, color2);
	painter.fillRect(areaWidth * 3, 0, areaWidth, areaHeight, color3);
	painter.fillRect(areaWidth * 4, 0, areaWidth, areaHeight, color4);
	painter.fillRect(areaWidth * 5, 0, areaWidth, areaHeight, color5);
	painter.fillRect(areaWidth * 6, 0, areaWidth, areaHeight, color6);
	painter.fillRect(areaWidth * 7, 0, areaWidth, areaHeight, color7);
	painter.fillRect(areaWidth * 8, 0, areaWidth, areaHeight, color8);
	painter.fillRect(areaWidth * 9, 0, areaWidth, areaHeight, color9);
	painter.fillRect(areaWidth * 10, 0, areaWidth, areaHeight, color10);

	//Write the values associated with the colours
	QRectF fontRect(0, 0, areaWidth, areaHeight);
	painter.setPen(fontColor);
	painter.setFont(QFont("Arial", 10));
	painter.drawText(fontRect, Qt::AlignCenter, "0");

	fontRect.moveTopLeft( QPointF(areaWidth * 1, 0) );
	painter.drawText(fontRect, Qt::AlignCenter, QString::number( Util::rDouble( 1*(maxValue/10.0), 1 ) ) );
	fontRect.moveTopLeft( QPointF(areaWidth * 2, 0) );
	painter.drawText(fontRect, Qt::AlignCenter, QString::number( Util::rDouble( 2*(maxValue/10.0), 1 ) ) );
	fontRect.moveTopLeft( QPointF(areaWidth * 3, 0) );
	painter.drawText(fontRect, Qt::AlignCenter, QString::number( Util::rDouble( 3*(maxValue/10.0), 1 ) ) );
	fontRect.moveTopLeft( QPointF(areaWidth * 4, 0) );
	painter.drawText(fontRect, Qt::AlignCenter, QString::number( Util::rDouble( 4*(maxValue/10.0), 1 ) ) );
	fontRect.moveTopLeft( QPointF(areaWidth * 5, 0) );
	painter.drawText(fontRect, Qt::AlignCenter, QString::number( Util::rDouble( 5*(maxValue/10.0), 1 ) ) );
	fontRect.moveTopLeft( QPointF(areaWidth * 6, 0) );
	painter.drawText(fontRect, Qt::AlignCenter, QString::number( Util::rDouble( 6*(maxValue/10.0), 1 ) ) );
	fontRect.moveTopLeft( QPointF(areaWidth * 7, 0) );
	painter.drawText(fontRect, Qt::AlignCenter, QString::number( Util::rDouble( 7*(maxValue/10.0), 1 ) ) );
	fontRect.moveTopLeft( QPointF(areaWidth * 8, 0) );
	painter.drawText(fontRect, Qt::AlignCenter, QString::number( Util::rDouble( 8*(maxValue/10.0), 1 ) ) );
	fontRect.moveTopLeft( QPointF(areaWidth * 9, 0) );
	painter.drawText(fontRect, Qt::AlignCenter, QString::number( Util::rDouble( 9*(maxValue/10.0), 1 ) ) );
	fontRect.moveTopLeft( QPointF(areaWidth * 10, 0) );
	painter.drawText(fontRect, Qt::AlignCenter, QString::number( Util::rDouble( maxValue, 1 ) ) );
}


