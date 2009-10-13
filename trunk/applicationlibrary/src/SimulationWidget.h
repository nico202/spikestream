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

#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

//SpikeStream includes
#include "SimulationManager.h"
#include "DBInterface.h"
#include "MonitorArea.h"
#include "BusyDialog.h"
#include "NeuronParametersDialog.h"
#include "SynapseParametersDialog.h"
#include "GlobalParametersDialog.h"
#include "ScriptRunner.h"
#include "NoiseParametersDialog.h"
#include "MonitorDataPlotter.h"
#include "GlobalVariables.h"

//Qt includes
#include <qwidget.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qstringlist.h>
#include <qradiobutton.h>
#include <q3table.h>
#include <q3groupbox.h>
#include <qlayout.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qvalidator.h>
#include <qapplication.h>


/*! Comparison function for simulation error map. */
struct qstringCompare{
  bool operator()(const QString* s1, const QString* s2) const {
    return strcmp(s1->ascii(), s2->ascii()) < 0;
  }
};


//------------------------ Simulation Widget -------------------------------
/*! Used to control simulation, adjust simulation parameters and visualise 
	what is going on in the neural networks.*/

/*FIXME Setting parameters could do with a check similar to that used for
	viewing and saving weights. */
//--------------------------------------------------------------------------

class SimulationWidget : public QWidget {
	Q_OBJECT
 
	public:
 		SimulationWidget(QWidget *parent, DBInterface*, DBInterface*, DBInterface*, DBInterface*);
		~SimulationWidget();
		SimulationManager* getSimulationManager();
		static unsigned int getSimulationStartTime();
		void hideOpenWindows();
		void reloadConnectionGroups();
		void reloadDevices();
		void reloadNeuronGroups();
		void reloadPatterns();
		void setFromNeuronID(unsigned int neurGrpID, unsigned int fromNeurID);
		void setMonitorArea(MonitorArea*);
		void setToNeuronID(unsigned int toNeurID);
		void showOpenWindows();


 	private slots:
		void checkSimManagerForErrors();
		void checkViewWeightsSaved();
		void checkWeightsLoadState();
		void checkWeightsSaveState();
		void dockAllButtonClicked();
		void fireNeuronButtonPressed();
		void fireNeuronComboActivated(const QString &string);
		void frameRateComboChanged(int);
		void globalParamButtonPressed();
		void hideGraphsButtonClicked();
		void initialiseButtonToggled(bool);
		void injectNoiseButtonPressed();
		void monitorButtonPressed();
		void monitorNeuronButtonPressed();
		void monitorNeuronComboActivated(const QString &string);
		void monitorSynapseButtonPressed();
		void neuronParamButtonPressed();
		void noiseParamButtonPressed();
		void recordButtonToggled(bool);
		void reloadWeightsButtonPressed();
		void saveWeightsButtonPressed();
		void setSimulationStartTime(unsigned int simStartTime);
		void showGraphsButtonClicked();
		void simModeComboChanged(int);
		void simStartButtonToggled(bool);
		void simStepButtonPressed();
		void simStopButtonPressed();
		void synapseParamButtonPressed();
		void undockAllButtonClicked();
		void viewWeightsButtonPressed();


	private:
		//======================== VARIABLES ==============================
		//References to database handling classes
		DBInterface *networkDBInterface;
		DBInterface *archiveDBInterface;
		DBInterface *patternDBInterface;
		DBInterface *deviceDBInterface;

		//Positions of columns that need to be referenced later
		int deviceNeurGrpCol;
		int deviceFiringModeCol;
		int patternNeurGrpCol;
		int deviceIDCol;
		int patternIDCol;

		/*! Reference to monitor area for docking and undocking of monitor windows.*/
		MonitorArea *monitorArea;

		/*! Reference to simulation manager to control simulation.*/
		SimulationManager *simulationManager;

		/*! Short version of reference to QApplication for processing events.*/
		SpikeStreamApplication *spikeStrApp;

		/*! Sometimes will need to change initialise button's state
			without doing anything.*/
		bool ignoreInitialiseButton;

		/*! Sometimes will need to change record button's state
			without doing anything.*/
		bool ignoreRecordButton;
		
		/*! Start time of simulation. Used by the load archive dialog
			to prevent deletion of archives that are currently being
			accessed by simulation.*/
		static unsigned int simulationStartTime;

		//Qt widgets for simulation settings
		Q3GroupBox *settingsGrpBox;
		QLineEdit *archNameText;
		QComboBox *archMonitorTypeCombo;
		QTabWidget *inputTabWidget;
		Q3Table *patternTable;
		Q3Table *deviceTable;
		QLineEdit *timeStepsPatternText;

		//Qt widgets for live parameters
		NeuronParametersDialog *neuronParametersDialog;
		SynapseParametersDialog *synapseParametersDialog;
		GlobalParametersDialog *globalParametersDialog;
		NoiseParametersDialog *noiseParametersDialog;

		//Qt widgets for simulation controls
		QPushButton *initialiseButton;
		QPushButton *reloadWeightsButton;
		QPushButton *saveWeightsButton;
		QPushButton *simStartButton;
		QPushButton *simStepButton;
		QPushButton *simStopButton;
		QPushButton *recordButton;
		QComboBox *neuronGrpMonitorCombo;
		QComboBox *liveMonitorTypeCombo;
		QPushButton *monitorButton;
		QPushButton *injectNoiseButton;
		QComboBox *injectNoiseNeurGrpCombo;
		QComboBox *injectNoiseAmntCombo;
		QPushButton *viewWeightsButton;
		QComboBox *frameRateCombo;
		QComboBox* simModeCombo;

		//Fire neuron widgets and variables
		QComboBox *fireNeuronCombo;
		QLineEdit *fireNeuronText;
		QPushButton *fireNeuronButton;
		QIntValidator *fireNeuronValidator;
		unsigned int maxFireNeuronID;
		unsigned int minFireNeuronID;

		//Monitor neuron widgets and variables
		QComboBox *monitorNeuronCombo;
		QLineEdit *monitorNeuronText;
		QPushButton *monitorNeuronButton;
		QIntValidator *monitorNeuronValidator;
		unsigned int maxMonitorNeuronID;
		unsigned int minMonitorNeuronID;

		//Monitor synapse widgets and variables
		QLabel* monitorSynapseFromLabel;
		QLabel* monitorSynapseToLabel;
		QLabel* monitorSynapseFromNumLabel;
		QLabel* monitorSynapseToNumLabel;
		QPushButton *monitorSynapseButton;

		//Button to show/hide graphs
		QPushButton *hideGraphsButton;
		QPushButton *showGraphsButton;

		//Qt Docking controls
		QPushButton *dockAllButton;
		QPushButton *undockAllButton;

		/*! Dialog to show when something is happening whose progress cannot be measured.*/
		BusyDialog *busyDialog;

		/*! Timer to check simulation manager for errors whilst simulation is running.*/
		QTimer* simErrorCheckTimer;

		/*! Map of the errors received from the simulation manager
			designed for error filtering.*/
		map<QString*, bool, qstringCompare> simulationErrorMap;

		/*! Map holding all of the monitors plotting neuron data.*/
		map<unsigned int, MonitorDataPlotter*> neuronMonitorMap;

		/*! Map holding all of the monitors plotting synapse data.*/
		map<unsigned int*, MonitorDataPlotter*, synapseKeyCompare> synapseMonitorMap;


		//======================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		SimulationWidget (const SimulationWidget&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		SimulationWidget operator = (const SimulationWidget&);

		void getNeuronGrpNames(QStringList&, unsigned int w, unsigned int l);
		void loadDeviceTable();
		void loadNeuronGrpNames(QComboBox *comboBox);
		void loadPatternTable();
		bool neuronGroupsIsolated();
};


#endif//SIMULATIONWIDGET_H


