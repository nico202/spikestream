/***************************************************************************
 *   SpikeStream Application                                               *
 *   Copyright (C) 2005 by David Gamez                                     *
 *   david@davidgamez.eu                                                   *
 *   Version 0.1                                                           *
 ***************************************************************************/

/* ------------------------ Neuron Monitor ----------------------------------
	Displays graphs of parameters monitored by the neuron.
----------------------------------------------------------------------------*/

#ifndef NEURONMONITOR_H
#define NEURONMONITOR_H

//SpikeStream includes
#include "MonitorDataset.h"

//Qt includes
#include <qdialog.h>
#include <qlabel.h>
#include <qlayout.h>

//Qwt includes
#include <qwt_plot.h>

//Other includes
#include <vector>
using namespace std;

struct NewGraph {
	QString description;
	double rangeLow;
	double rangeHigh;
};

class NeuronMonitor : public QDialog {
	Q_OBJECT

	public: 
		NeuronMonitor(QWidget *parent, QString neuronGrpDesc, unsigned int neurID, unsigned int neurGrpID);
		~NeuronMonitor();
		void addGraph(NewGraph graph);
		void closeDialog();
		void loadingComplete();
		void plotData(double msgTime, const double* dataArray, int arrayLength);
		void setUpGraphs(const char* charArray);


	protected:
		void closeEvent ( QCloseEvent * e );


	private:
		//=============================== VARIABLES ===============================
		/* Store a reference to the simulation widget to access simulation manager
			Stored as a QWidget to avoid include problems. */
		QWidget* simulationWidget;

		//ID and group of neuron being monitored
		unsigned int neuronID;
		unsigned int neuronGrpID;

		//Label to inform user that we are waiting for simulation manager
		QLabel* loadingLabel;

		//Layout of this widget, used to dynamically add graphs
		QVBoxLayout *verticalBox;

		//Holds references to all of the graphs in this widget
		vector<QwtPlot*> plotVector;

		//Holds references to all of the datasets plotted by the graphs
		vector<MonitorDataset*> dataVector;

};


#endif//NEURONMONITOR_H


