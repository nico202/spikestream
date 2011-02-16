//SpikeStream includes
#include "MembranePotentialGraphWidget.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

/*! Constructor */
MembranePotentialGraphWidget::MembranePotentialGraphWidget(QWidget* parent) : AbstractGraphWidget(parent) {
	plotColor = qRgb(255, 0, 0);
	setYRange(-130, 50);
	buildBufferImage();

	newXPos = 0;
	newYPos = 0;
	oldXPos = 0;
	oldYPos = 0;
}


/*! Destructor */
MembranePotentialGraphWidget::~MembranePotentialGraphWidget(){

}



/*----------------------------------------------------------*/
/*------               PUBLIC METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Adds a membrane potential value to the plot */
void MembranePotentialGraphWidget::addData(float membranePotential, timestep_t timeStep){
	oldXPos = newXPos;
	oldYPos = newYPos;

	newXPos = timeStep % numTimeSteps +  yAxisPadding + 1;

	//Make sure membrane potential does not go out of range
	newYPos = imageHeight - Util::rInt(membranePotential - getMinY());
	if(newYPos > imageHeight)
		newYPos = imageHeight;
	if(newYPos < 0)
		newYPos = 0;

	//Advance the time step
	increaseTimeStep(timeStep);
	repaint();
}


/*! Paints a line between the last 2 data points */
void MembranePotentialGraphWidget::paintData(QPainter &painter){
	painter.setPen( QPen(plotColor) );
	//Only draw line if it is a sequential point
	if(newXPos - oldXPos == 1)
		painter.drawLine(oldXPos, oldYPos, newXPos, newYPos);
}


/*! Paints the labels  */
void MembranePotentialGraphWidget::paintLabels(QPainter& painter){
}
