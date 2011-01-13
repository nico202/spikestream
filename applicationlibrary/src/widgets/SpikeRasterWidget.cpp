//SpikeStream includes
#include "SpikeRasterWidget.h"
using namespace spikestream;

//Qt includes
#include <QPainter>
#include <QTimer>
#include <QResizeEvent>
#include <QTextStream>
#include <QDebug>

//! \todo remove
#include <iostream>
#include <assert.h>


/*! Constructor */
SpikeRasterWidget::SpikeRasterWidget(QList<NeuronGroup*>& neuronGroupList, QWidget* parent) : 	QWidget(parent),
	m_updates(0),
	imageBufferVector(1), //! \todo set this based on size
	m_yscale(1.0),
	m_cycle(-1),
	m_fillCycle(0),
	m_axisPadding(0),
	m_axisTickSize(4)
{

	//Initialize variables
	readIndex = 0;
	writeIndex = 0;
	numTimeSteps = 1000;
	minTimeStep = 0;
	incrementReadIndex = false;
	bufferWidth = 1;

	//Display variables
	xAxisPadding = 5;
	yAxisPadding = 5;
	xAxisTickLength = 6;
	yAxisTickLength = 6;
	fontSize = 10;

	//Store list of neuron groups that are being monitored
	this->neuronGroupList = neuronGroupList;

	//Calculate number of neurons
	numNeurons = 0;
	for(QList<NeuronGroup*>::iterator iter = neuronGroupList.begin(); iter != neuronGroupList.end(); ++iter)
		numNeurons += (*iter)->size();

	//Create buffers
	for(int b=0; b < numTimeSteps; ++b) {
		QImage buffer(bufferWidth, numNeurons + fontSize + xAxisPadding, QImage::Format_RGB32);
		buffer.fill(QColor(255,255,255).rgb());
		imageBufferVector.push_back(buffer);
	}

	//Set up widget graphic properties
	setMinimumSize(numTimeSteps, 500);
	//setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setAttribute(Qt::WA_OpaquePaintEvent);
	//setAttribute(Qt::WA_PaintOnScreen); // faster, but more flicker
	widgetWidth = this->size().width();
	widgetHeight = this->size().height();
}


/*! Destructor */
SpikeRasterWidget::~SpikeRasterWidget(){
}


/*----------------------------------------------------------*/
/*------               PUBLIC METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Adds spikes to the plot, filtered by the neuron groups that are being monitored. */
void SpikeRasterWidget::addSpikes(const QList<unsigned>& firingNeuronIDs, int timeStep){

	//Add spikes to the current image
	qDebug()<<"Updating spikes. Number of spikes: "<<firingNeuronIDs.size()<<"; timestep="<<timeStep;
	unsigned tmpNeurID;
	QList<unsigned>::const_iterator firingNeuronIDsEnd = firingNeuronIDs.end();
	for(QList<unsigned>::const_iterator neurIter = firingNeuronIDs.begin(); neurIter != firingNeuronIDsEnd; ++neurIter){
		for(QList<NeuronGroup*>::iterator neurGrpIter = neuronGroupList.begin(); neurGrpIter != neuronGroupList.end(); ++neurGrpIter){
			if((*neurGrpIter)->contains(*neurIter)){
				//addSpike(timeStep, ));//FIXME: ADD OFFSET FOR EACH NEURON GROUP
				tmpNeurID = *neurIter - (*neurGrpIter)->getStartNeuronID();
				//qDebug()<<"Painting neuron at: (0, "<<tmpNeurID<<")";
				imageBufferVector[writeIndex].setPixel(0, tmpNeurID, qRgb(255, 0, 0));
				break;
			}
		}
	}

	//Advance the time step
	increaseTimeStep(timeStep);

	repaint();
}



/*! Spikes are ignored if they are out of bounds, either spatially or
 * temporally */
void SpikeRasterWidget::addSpike(int cycle, int neuron) {
	qDebug()<<"cycle: "<<cycle<<"; neuronID: "<<neuron<<"; numNeurons: "<<numNeurons;
	assert(cycle >= m_fillCycle);
	assert(cycle < m_fillCycle + numTimeSteps);
	assert(neuron < numNeurons);

	if(cycle >= m_fillCycle && cycle < m_fillCycle + numTimeSteps && neuron < numNeurons) {
		imageBufferVector[writeIndex].setPixel(0, neuron, qRgb(0, 0, 0));
	}
	else {
		assert(false);
	}
}


/*----------------------------------------------------------*/
/*------              PRIVATE METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Advances the time step for reading and writing */
void SpikeRasterWidget::increaseTimeStep(int currentTimeStep){
	//Increase write index in a circular fashion
	++writeIndex;
	writeIndex %= numTimeSteps;

	//Increase read index if we have done at least one revolution
	if(writeIndex == 0 && !incrementReadIndex)//Have done at least one revolution
		incrementReadIndex = true;
	else if(incrementReadIndex){
		++readIndex;
		readIndex = minTimeStep % numTimeSteps;
	}

	//Record the minimum time step for the graph axis
	if(currentTimeStep > numTimeSteps)
		minTimeStep = currentTimeStep - numTimeSteps;
}


//Inherited from QWidget
void SpikeRasterWidget::resizeEvent(QResizeEvent* /*event*/) {
	qDebug()<<"Resizing: ";
	widgetWidth = this->size().width();
	widgetHeight = this->size().height();
}


/*! Paints a single tick on the Y axis */
void SpikeRasterWidget::paintYAxisTick(QPainter& painter, int yPos, int label) {
	int textW = fontSize*5;
	if(yPos + textW > height())
		return;

	painter.drawLine(yAxisPadding - yAxisTickLength/2 + fontSize, yPos, yAxisPadding + yAxisTickLength/2 + fontSize, yPos);

	painter.save();
	painter.rotate(3.14);
	painter.drawText(0, yPos - textW/2,
			textW, fontSize,
			Qt::AlignHCenter,
			QString::number(label) + "ms"
	);
	painter.restore();
}


/*! Paints the Y axis */
void SpikeRasterWidget::paintYAxis(QPainter& painter){
	//Draw Y axis line
	painter.drawLine(yAxisPadding + fontSize, 0, yAxisPadding + fontSize, height());

	//Draw ticks
	for(int i=0; i<numNeurons; i += numNeurons/10)
		paintYAxisTick(painter, i + xAxisPadding + fontSize, i);
}


/*! Paints a single tick on the X axis */
void SpikeRasterWidget::paintXAxisTick(QPainter& painter, int xPos, int labelX) {
	xPos += (yAxisPadding + fontSize);
	painter.drawLine(xPos,
					 height()-xAxisPadding - xAxisTickLength/2 - fontSize,
					 xPos,
					 height()-xAxisPadding + xAxisTickLength/2 - fontSize
	);
	int textW = fontSize*5;
	painter.drawText(xPos - textW/2, height()-fontSize,
			textW, fontSize,
			Qt::AlignHCenter,
			QString::number(labelX) + "ms"
	);
}


/*! set axis ticks for every 50ms */
void SpikeRasterWidget::paintXAxis(QPainter& painter) {
	//Draw X axis line
	painter.drawLine(0, height()-xAxisPadding - fontSize, width(), height()-xAxisPadding - fontSize);

	//Draw ticks
	for(int i=0; i<numTimeSteps; i += numTimeSteps/10)
		paintXAxisTick(painter, i + yAxisPadding, minTimeStep + i);
}


/*! Paints the X and Y axes */
void SpikeRasterWidget::paintAxes(QPainter& painter) {
	painter.setPen( QPen(Qt::black) );

	QFont font; 
	font.setFamily("Helvetica");
	font.setWeight(QFont::Light);
	font.setPixelSize(fontSize);
	painter.setFont(font);

	paintYAxis(painter);
	paintXAxis(painter);
}


void SpikeRasterWidget::paintData(QPainter& painter) {
//	int w = imageBufferVector.first().width();
//	for(int i=0; i < imageBufferVector.size(); ++i){
//		int currentBuffer = (readIndex + i ) % imageBufferVector.size();
//		painter.drawImage(i*w, 0, imageBufferVector[currentBuffer]);
//	}

	//int w = imageBufferVector.first().width();
	qDebug()<<"Widget height: "<<widgetHeight;
	for(int i=0; i < imageBufferVector.size(); ++i){
		int currentBufferIndex = (readIndex + i ) % imageBufferVector.size();
		painter.drawImage(i + fontSize + yAxisPadding, 0, imageBufferVector[currentBufferIndex]);
//		qDebug()<<"Current buffer index: "<<currentBufferIndex;
//		QImage tmpImage = imageBufferVector[currentBufferIndex].scaled(1, widgetHeight, Qt::IgnoreAspectRatio, Qt::FastTransformation);
//		if(tmpImage.isNull())
//			qDebug()<<"NULL IMAGE";
//		else{
//			qDebug()<<"Scaled image: width="<<tmpImage.width()<<"; height = "<<tmpImage.height();
//			painter.drawImage(i, 0, tmpImage);
//		}
	}
}


void SpikeRasterWidget::paintEvent(QPaintEvent*){
	m_updates += 1;
	QPainter painter(this);
	paintData(painter);
	paintAxes(painter);
	painter.end();
}
