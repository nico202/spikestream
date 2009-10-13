/***************************************************************************
 *   SpikeStream Application                                               *
 *   Copyright (C) 2005 by David Gamez                                     *
 *   david@davidgamez.eu                                                   *
 *   Version 0.1                                                           *
 ***************************************************************************/

//SpikeStream includes
#include "NeuronMonitor.h"
#include "Debug.h"
#include "NeuronMonitorXmlHandler.h"
#include "SpikeStreamMainWindow.h"
#include "SimulationWidget.h"

//Qt includes
#include <qlayout.h>
#include <qxml.h>
#include <qmessagebox.h>
#include <qcolor.h>
#include <qpainter.h>

//Qwt includes
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>

//Other includes
#include <iostream>
using namespace std;


//Number of items in dataset
#define DEFAULT_MONITOR_DATASET_SIZE 100


/* Class that draws the background of the plot - easier to put it here than a separate file! */
class Background: public QwtPlotItem {
	public:
		Background(){
			setZ(0.0);
			QColor colour(255, 255, 255);
			brush = new QBrush(colour, Qt::SolidPattern);
		}
		~Background(){
			delete brush;
		}

		virtual int rtti() const {
			return QwtPlotItem::Rtti_PlotUserItem;
		}

		virtual void draw(QPainter *painter, const QwtScaleMap &, const QwtScaleMap &yMap, const QRect &rect) const {
			painter->fillRect(rect, *brush);
		}
	private:
		QBrush* brush;
};


/* Constructor for monitoring neuron data
	The main initialisation work is done when the setUpGraphs method is called after the information
	about the monitored data has been received by the simulationManager. */
NeuronMonitor::NeuronMonitor(QWidget *parent, QString neuronGrpDesc, unsigned int neurID, unsigned int neurGrpID) : QDialog(parent, "NeurMonDlg", false){
	neuronMonitoring = true;
	
	//Store neuron ID and group
	neuronID = neurID;
	neuronGrpID = neurGrpID;

	//Store parent class as a widget to avoid include problems
	simulationWidget = parent;

	//Set caption
	QString captionStr = "Monitoring neuron ";
	captionStr += QString::number(neurID) += " in neuron group ";
	captionStr += neuronGrpDesc;
	this->setCaption(captionStr);

	//Create box to organise dialog
	verticalBox = new QVBoxLayout(this, 2, 2);

	//Add temporary label explaining that we are waiting for neuron information.
	loadingLabel = new QLabel("Loading graphs, please wait.", this);
	verticalBox->addWidget(loadingLabel);

	this->show();
}

/* Constructor for monitoring synapse data
	The main initialisation work is done when the setUpGraphs method is called after the information
	about the monitored data has been received by the simulationManager. */
NeuronMonitor::NeuronMonitor(QWidget *parent, unsigned int neurGrpID, unsigned int fromNeurID, unsigned int toNeurID) : QDialog(parent, "NeurMonDlg", false){
	neuronMonitoring = false;

	//Store neuron ID and group
	fromNeuronID = fromNeurID;
	toNeuronID = toNeurID;
	neuronGrpID = neurGrpID;

	//Store parent class as a widget to avoid include problems
	simulationWidget = parent;

	//Set caption
	QString captionStr = "Monitoring synapse ";
	captionStr += QString::number(neurID) += " in neuron group ";
	captionStr += neuronGrpDesc;
	this->setCaption(captionStr);

	//Create box to organise dialog
	verticalBox = new QVBoxLayout(this, 2, 2);

	//Add temporary label explaining that we are waiting for neuron information.
	loadingLabel = new QLabel("Loading graphs, please wait.", this);
	verticalBox->addWidget(loadingLabel);

	this->show();
}

//Destructor
NeuronMonitor::~NeuronMonitor(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING MONITOR DATA PLOTTER"<<endl;
	#endif//MEMORY_DEBUG
}


//--------------------------------------------------------------------------------
//----------------------      PUBLIC METHODS       -------------------------------
//--------------------------------------------------------------------------------

/* Adds a graph to the display. Usually called by NeuronMonitorXmlHandler, which in turn is invoked 
	by simulationManager running as a separate thread. */
void NeuronMonitor::addGraph(NewGraph newGraph){
	cout<<"MONITOR DATA PLOTTER ADDING GRAPH "<<newGraph.description<<" range from "<<newGraph.rangeLow<<" to "<<newGraph.rangeHigh<<endl;

	//Lock mutex because this method is invoked by a separate thread
	SpikeStreamMainWindow::spikeStreamApplication->lock();

	//Create a plot
	QwtPlot* tempPlot = new QwtPlot(this, "Temp Curve");
	verticalBox->addWidget(tempPlot);

	//Set titles on the axes
    tempPlot->setAxisTitle(QwtPlot::yLeft, newGraph.description);
	tempPlot->setAxisTitle(QwtPlot::xBottom, "Time (ms)");

	//Add background
    Background *bg = new Background();
    bg->attach(tempPlot);

	//Create a curve for the plot
	QwtPlotCurve *tempCurve = new QwtPlotCurve("Curve 1");
	QColor redColor(255, 0, 0);
	QBrush tmpBrush(redColor, Qt::Dense4Pattern);
    tempCurve->setBrush(tmpBrush);
	QPen tmpPen(redColor);
	tempCurve->setPen(tmpPen);

	//Create a new dataset to manage the data for this graph
	MonitorDataset* tempData = new MonitorDataset(DEFAULT_MONITOR_DATASET_SIZE, newGraph.rangeLow, newGraph.rangeHigh);

	//Pass a reference to the dataset to the curve
	tempCurve->setData(*tempData);
	
	//Attach the curve to the plot
	tempCurve->attach(tempPlot);
	
	//Refresh the plot
	tempPlot->replot();

	//Make plot visible.
	tempPlot->show();

	//Store reference to new dataset
	dataVector.push_back(tempData);

	//Store reference to new plot
	plotVector.push_back(tempPlot);

	//Unlock mutex
	SpikeStreamMainWindow::spikeStreamApplication->unlock();
}


//Called when the dialog needs to be closed by an external class. */
void NeuronMonitor::closeDialog(){
	cout<<"MONITOR DATA PLOTTER CLOSING closeDialog()"<<endl;
	this->accept();
}


/* Adds the data in the float array to the graphs */
void NeuronMonitor::plotData(double time, const double* dataArray, int arrayLength){
	//Lock mutex because this method is invoked by a separate thread
	SpikeStreamMainWindow::spikeStreamApplication->lock();

	//Double check that arrayLength is the same as the vector lengths
	if(arrayLength != plotVector.size()){
		QMessageBox::critical(this, "Monitor Error", "Data and graphs do not match!");
		return;
	}

	//Add the data to the plots and replot the graph
	for(int i=0; i<arrayLength; ++i){
		dataVector[i]->addPoint(time, dataArray[i]);
		plotVector[i]->replot();
	}

	//Unlock mutex
	SpikeStreamMainWindow::spikeStreamApplication->unlock();
}	


/* The char array is an XML file containing the set up information for the graphs.
	This method parses the information and creates the graphs. */
void NeuronMonitor::setUpGraphs(const char* charArray){
	QString xmlString(charArray);
	QXmlInputSource xmlInput;
	xmlInput.setData(xmlString);
	MonitorXmlHandler monXmlHandler(this);
    QXmlSimpleReader reader;
	reader.setContentHandler(&monXmlHandler);
	reader.setErrorHandler(&monXmlHandler);
	reader.parse(xmlInput);
}


/* Called when all graphs have been loaded so that loading label can be hidden. */
void NeuronMonitor::loadingComplete(){
	loadingLabel->hide();
}

//--------------------------------------------------------------------------------
//----------------------      PROTECTED METHODS       ----------------------------
//--------------------------------------------------------------------------------

/* Called when user closes the dialog. Instructs simulation to stop sending monitor data. 
	Dialog is closed programmatically by the closeDialog() method, which may trigger this. */

void NeuronMonitor::closeEvent ( QCloseEvent * e ){
	cout<<"NEURON MONITOR CLOSING closeEvent()"<<endl;

	//Lock mutex because this method is invoked by a separate thread
	SpikeStreamMainWindow::spikeStreamApplication->lock();

	//Tell simulationManager to stop monitoring neuron or synapse
	if(neuronMonitoring)
		((SimulationWidget*)simulationWidget)->getSimulationManager()->stopNeuronMonitoring(neuronGrpID, neuronID);
	else
		((SimulationWidget*)simulationWidget)->getSimulationManager()->stopSynapseMonitoring(neuronGrpID, fromNeuronID, toNeuronID);

	//Unlock mutex
	SpikeStreamMainWindow::spikeStreamApplication->unlock();

	this->accept();
}



