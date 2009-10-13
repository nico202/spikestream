/***************************************************************************
 *   SpikeStream Application                                               *
 *   Copyright (C) 2007 by David Gamez                                     *
 *   david@davidgamez.eu                                                   *
 *   Version 0.1                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//SpikeStream includes
#include "MonitorDataPlotter.h"
#include "Debug.h"
#include "MonitorXmlHandler.h"
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
//Added by qt3to4:
#include <QCloseEvent>
#include <QLabel>
#include <Q3VBoxLayout>

//Other includes
#include <iostream>
using namespace std;


/*! Default number of items in dataset. This controls the resolution of the X axis in the plot. */
#define DEFAULT_MONITOR_DATASET_SIZE 100


/*! Class that draws the background of the plot - easier to put it here than a separate file. */
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

		virtual void draw(QPainter *painter, const QwtScaleMap&, const QwtScaleMap&, const QRect &rect) const {
			painter->fillRect(rect, *brush);
		}
	private:
		QBrush* brush;
};


/*! Constructor for monitoring neuron data
	The main initialisation work is done when the setUpGraphs method is called after the information
	about the monitored data has been received by the simulationManager. */
MonitorDataPlotter::MonitorDataPlotter(QWidget *parent, QString neuronGrpDesc, unsigned int neurGrpID, unsigned int neurID) : QDialog(parent, "NeurMonDlg", false){
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
	verticalBox = new Q3VBoxLayout(this, 2, 2);

	//Add temporary label explaining that we are waiting for neuron information.
	loadingLabel = new QLabel("Loading graphs, please wait.", this);
	verticalBox->addWidget(loadingLabel);

	//Create font for axes
	axisFont = new QFont( "Arial", 8);
	axisTitleFont = new QFont("Arial", 9, QFont::Bold);

	//Initialise variables
	parseError = false;

	this->show();
}


/*! Constructor for monitoring synapse data
	The main initialisation work is done when the setUpGraphs method is called after the information
	about the monitored data has been received by the simulationManager. */
MonitorDataPlotter::MonitorDataPlotter(QWidget *parent, unsigned int neurGrpID, unsigned int fromNeurID, unsigned int toNeurID) : QDialog(parent, "MonDlg", false){
	neuronMonitoring = false;

	//Store neuron ID and group
	fromNeuronID = fromNeurID;
	toNeuronID = toNeurID;
	neuronGrpID = neurGrpID;

	//Store parent class as a widget to avoid include problems
	simulationWidget = parent;

	//Set caption
	QString captionStr = "Monitoring synapse from ";
	captionStr += QString::number(fromNeuronID) += " to ";
	captionStr += QString::number(toNeuronID) += " in neuron group ";
	captionStr += QString::number(neuronGrpID);
	this->setCaption(captionStr);

	//Create box to organise dialog
	verticalBox = new Q3VBoxLayout(this, 2, 2);

	//Add temporary label explaining that we are waiting for neuron information.
	loadingLabel = new QLabel("Loading graphs, please wait.", this);
	verticalBox->addWidget(loadingLabel);

	//Create font for axes
	axisFont = new QFont( "Arial", 8);
	axisTitleFont = new QFont("Arial", 9, QFont::Bold);

	//Initialise variables
	parseError = false;

	this->show();
}


/*! Destructor. */
MonitorDataPlotter::~MonitorDataPlotter(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING MONITOR DATA PLOTTER"<<endl;
	#endif//MEMORY_DEBUG

	//Clean up plots. These should destroy their copies of the data structures
	for(vector<QwtPlot*>::iterator iter = plotVector.begin(); iter != plotVector.end(); ++iter)
		delete *iter;

	//Clean up data structures
	for(vector<MonitorDataset*>::iterator iter = dataVector.begin(); iter != dataVector.end(); ++iter){
		(*iter)->cleanUp();
		delete *iter;
	}
}


//--------------------------------------------------------------------------------
//----------------------      PUBLIC METHODS       -------------------------------
//--------------------------------------------------------------------------------

/*! Adds a graph to the display. Usually called by MonitorXmlHandler, which in turn is invoked 
	by simulationManager running as a separate thread. */
void MonitorDataPlotter::addGraph(NewGraph newGraph){
	#ifdef GRAPH_LOAD_DEBUG
		cout<<"MonitorDataPlotter: Adding graph: "<<newGraph.description<<" range from "<<newGraph.rangeLow<<" to "<<newGraph.rangeHigh<<endl;
	#endif//GRAPH_LOAD_DEBUG

	//Lock mutex because this method is invoked by a separate thread
	SpikeStreamMainWindow::spikeStreamApplication->lock();

	//Create a plot
        QwtPlot* tempPlot = new QwtPlot(QString("Temp Curve"), this);
	tempPlot->setMinimumSize(120, 120);

	//Set font of axes
	tempPlot->setAxisFont(QwtPlot::xBottom, *axisFont);
	tempPlot->setAxisFont(QwtPlot::yLeft, *axisFont);

	//Set up scale on Y axis to prevent replot. Scale on X axis will be set when adding points
	tempPlot->setAutoReplot(false);

	//Add to widget
	verticalBox->addWidget(tempPlot);

	//Set titles on the axes
	QwtText yText(newGraph.description);
	yText.setFont(*axisTitleFont);
    tempPlot->setAxisTitle(QwtPlot::yLeft, yText);

	QwtText xText("Time (ms)");
	xText.setFont(*axisTitleFont);
	tempPlot->setAxisTitle(QwtPlot::xBottom, xText);

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


/*! Called when the dialog needs to be closed by an external class.
	Variable stopMon determines whether monitoring will be stopped when dialog is closed. */
void MonitorDataPlotter::closeDialog(bool stopMon){

	//Stop the monitoring
	if(stopMon)
		stopMonitoring();

	//Accept the closure of the dialog
	this->accept();
}


/*! Called when all graphs have been loaded so that loading label can be hidden. */
void MonitorDataPlotter::loadingComplete(){
	#ifdef GRAPH_LOAD_DEBUG
		cout<<"MonitorDataPlotter: Graph loading complete."<<endl;
	#endif//GRAPH_LOAD_DEBUG

	loadingLabel->hide();
}


/*! Adds the data in the float array to the graphs. */
void MonitorDataPlotter::plotData(double time, const double* dataArray, int arrayLength){
	//Lock mutex because this method is invoked by a separate thread
	SpikeStreamMainWindow::spikeStreamApplication->lock();

	//Double check that arrayLength is the same as the vector lengths
	if((unsigned int)arrayLength != plotVector.size()){
		QMessageBox::critical(this, "Monitor Error", "Data and graphs do not match!");
		return;
	}

	//Add the data to the plots and replot the graph
	for(int i=0; i<arrayLength; ++i){
		dataVector[i]->addPoint(time, dataArray[i]);

		//Adjust the X axis to fit
		QwtDoubleRect* bndRect = dataVector[i]->boundingRectRef();
		plotVector[i]->setAxisScale(QwtPlot::xBottom, bndRect->left(), bndRect->left() + bndRect->width());

		//Replot graph
		plotVector[i]->replot();
	}

	//Unlock mutex
	SpikeStreamMainWindow::spikeStreamApplication->unlock();
}	


/*! Used by other methods to make the dialog visible and restart the monitoring. */
bool MonitorDataPlotter::showDialog(){
	if(parseError){//Dialog has already been launched and generated a parsing error
		QMessageBox::critical( 0, "Monitor Error", "Dialog cannot be shown if it has generated a parsing error");
		return false;
	}

	bool restartOk = true;

	//Lock mutex in case this method is invoked by a separate thread
	SpikeStreamMainWindow::spikeStreamApplication->lock();

	//Tell simulationManager to start monitoring neuron or synapse
	if(neuronMonitoring){
		if(!((SimulationWidget*)simulationWidget)->getSimulationManager()->startNeuronMonitoring(neuronGrpID, neuronID, true)){
			cerr<<"MonitorDataPlotter: ERROR RESTARTING NEURON MONITORING FOR NEURON GROUP "<<neuronGrpID<<" AND NEURON "<<neuronID<<endl;
			restartOk = false;
		}
	}
	else{
		if(!((SimulationWidget*)simulationWidget)->getSimulationManager()->startSynapseMonitoring(neuronGrpID, fromNeuronID, toNeuronID, true)){
			cerr<<"MonitorDataPlotter: ERROR RESTARTING SYNAPSE MONITORING FOR NEURON GROUP "<<neuronGrpID<<" FROM NEURON "<<fromNeuronID<<" TO "<<toNeuronID<<endl;
			restartOk = false;
		}
	}

	//Unlock mutex
	SpikeStreamMainWindow::spikeStreamApplication->unlock();

	//Show dialog
	this->show();

	//Return false if there has been an error resetarting.
	return restartOk;
}


/*! The char array is an XML file containing the set up information for the graphs.
	This method parses the information and creates the graphs. */
void MonitorDataPlotter::setUpGraphs(const char* charArray){
	//Lock mutex in case this method is invoked by a separate thread
	SpikeStreamMainWindow::spikeStreamApplication->lock();

	#ifdef GRAPH_LOAD_DEBUG
		cout<<"MonitorDataPlotter: Parsing XML: "<<charArray<<endl;
	#endif//GRAPH_LOAD_DEBUG

	QString xmlString(charArray);
	try{
		QXmlInputSource xmlInput;
		xmlInput.setData(xmlString);
		MonitorXmlHandler monXmlHandler(this);
		QXmlSimpleReader reader;
		reader.setContentHandler(&monXmlHandler);
		reader.setErrorHandler(&monXmlHandler);
		reader.parse(xmlInput);
		if(monXmlHandler.getParseError()){
			//Display error
                        cout<<"MonitorDataPlotter: ERROR OCCURRED DURING PARSING \""<<monXmlHandler.getParseErrorString().toStdString()<<"\""<<endl;
			QString errorString = "Error encountered whilst parsing XML monitor description: \"";
			errorString += monXmlHandler.getParseErrorString();
			errorString += "\"";
			QMessageBox::critical( 0, "Monitor Error", errorString);

			//Record that we have an error
			parseError = true;

			//Stop moinitoring
			stopMonitoring();
		
			//Accept the closure of the dialog
			this->accept();
		}
	}
	catch (std::exception& er) {// Catch-all for any other exceptions
		//Display error
		cerr<<"MonitorDataPlotter: EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown parsing XML monitor description: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Monitor Error", errorString);

		//Record that we have an error
		parseError = true;

		//Stop moinitoring
		stopMonitoring();
	
		//Accept the closure of the dialog
		this->accept();
	}

	//Unlock mutex
	SpikeStreamMainWindow::spikeStreamApplication->unlock();

}


//--------------------------------------------------------------------------------
//----------------------      PROTECTED METHODS       ----------------------------
//--------------------------------------------------------------------------------

/*! Called when user closes the dialog. Instructs simulation to stop sending monitor data. 
	Dialog is closed programmatically by the closeDialog() method. */
void MonitorDataPlotter::closeEvent ( QCloseEvent*){

	//Stop moinitoring
	stopMonitoring();

	//Accept the closure of the dialog
	this->accept();
}


//--------------------------------------------------------------------------------
//----------------------      PRIVATE  METHODS       -----------------------------
//--------------------------------------------------------------------------------

/*! Stops the monitoring associated with this dialog. */
void MonitorDataPlotter::stopMonitoring(){

	//Lock mutex because this method is invoked by a separate thread
	SpikeStreamMainWindow::spikeStreamApplication->lock();

	//Tell simulationManager to stop monitoring neuron or synapse
	if(neuronMonitoring)
		((SimulationWidget*)simulationWidget)->getSimulationManager()->stopNeuronMonitoring(neuronGrpID, neuronID);
	else
		((SimulationWidget*)simulationWidget)->getSimulationManager()->stopSynapseMonitoring(neuronGrpID, fromNeuronID, toNeuronID);

	//Unlock mutex
	SpikeStreamMainWindow::spikeStreamApplication->unlock();
}


