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

#ifndef MONITORDATAPLOTTER_H
#define MONITORDATAPLOTTER_H

//SpikeStream includes
#include "MonitorDataset.h"

//Qt includes
#include <qdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qfont.h>

//Qwt includes
#include <qwt_plot.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QCloseEvent>

//Other includes
#include <vector>
using namespace std;


/*! Holds the parameters associated with a new graph. */
struct NewGraph {
	QString description;//Description of the graph
	double rangeLow;//Default low range of the values
	double rangeHigh;//Default high range of the values
};


//------------------------ Monitor Data Plotter----------------------------
/*! Displays graphs of parameters monitored in a neuron or synapse. */
//-------------------------------------------------------------------------

class MonitorDataPlotter : public QDialog {
	Q_OBJECT

	public: 
		MonitorDataPlotter(QWidget *parent, QString neuronGrpDesc, unsigned int neurGrpID, unsigned int neurID);
		MonitorDataPlotter(QWidget *parent, unsigned int neurGrpID, unsigned int fromNeurID, unsigned int toNeurID);
		~MonitorDataPlotter();
		void addGraph(NewGraph graph);
		void closeDialog(bool stopMon);
		void loadingComplete();
		void plotData(double msgTime, const double* dataArray, int arrayLength);
		void setUpGraphs(const char* charArray);
		bool showDialog();


	protected:
		void closeEvent ( QCloseEvent * e );


	private:
		//=============================== VARIABLES ===============================
		/*! Store a reference to the simulation widget to access simulation manager
			Stored as a QWidget to avoid include problems. */
		QWidget* simulationWidget;

		/*! Neuron group.*/
		unsigned int neuronGrpID;

		/*! Are we monitoring a neuron or a synapse?.*/
		bool neuronMonitoring;

		/*! ID of neuron being monitored.*/
		unsigned int neuronID;

		/*! Presynaptic ID of synapse being monitored.*/
		unsigned int fromNeuronID;

		/*! Postsynpatic ID of synapse being monitored.*/
		unsigned int toNeuronID;

		/*! Label to inform user that we are waiting for simulation manager.*/
		QLabel* loadingLabel;

		/*! Layout of this widget, used to dynamically add graphs.*/
		Q3VBoxLayout *verticalBox;

		/*! Holds references to all of the graphs in this widget.*/
		vector<QwtPlot*> plotVector;

		/*! Holds references to all of the datasets plotted by the graphs.*/
		vector<MonitorDataset*> dataVector;

		/*! Records if there has been an error in parsing. */
		bool parseError;

		//Font for axes
		QFont* axisFont;
		QFont* axisTitleFont;


		//=============================== METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		MonitorDataPlotter (const MonitorDataPlotter&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		MonitorDataPlotter operator = (const MonitorDataPlotter&);

		void stopMonitoring();

};


#endif//MONITORDATAPLOTTER_H


