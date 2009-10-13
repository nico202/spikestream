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

#ifndef ARCHIVEWIDGET_H
#define ARCHIVEWIDGET_H

//SpikeStream includes
#include "ArchiveManager.h"
#include "DBInterface.h"
#include "MonitorArea.h"
#include "NeuronGroup.h"
#include "ArchiveStatistic.h"
#include "ViewModelDialog.h"

//Qt includes
#include <qwidget.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <q3groupbox.h>
#include <q3table.h>
//Added by qt3to4:
#include <QPixmap>


//-------------------------- Archive Widget --------------------------------
/*! Displays controls to play back archives that have been recorded. */
//--------------------------------------------------------------------------

class ArchiveWidget : public QWidget {
	Q_OBJECT

	public:
 		ArchiveWidget(QWidget *parent, DBInterface*, ArchiveManager*);
		~ArchiveWidget();
		ArchiveManager* getArchiveManager();
		void hideOpenWindows();
		void setMonitorArea(MonitorArea*);
		void showOpenWindows();


	public slots:
		void unloadArchive();

 
 	private slots:
		void addStatsButtonClicked();
		void archiveNameChanged(QString timeStamp, QString newName);
		void archiveStopped();
		void deleteStatsButtonClicked();
		void dockAllButtonClicked();
		void fastForwardButtonToggled(bool on);
		void frameRateComboChanged(int);
		void freezeArchive();
		void loadButtonPressed();
		void playButtonToggled(bool);
		void rewindButtonPressed();
		void setSpikeCount(unsigned int);
		void setSpikeTotal(unsigned int);
		void showArchiveError(const QString& message);
		void statsTableClicked(int row, int col, int, const QPoint &);
		void stepButtonPressed();
		void stopButtonPressed();
		void unfreezeArchive();
		void undockAllButtonClicked();
		void updateStatisticsTable();
		void viewModelButtonClicked();


	private:
		//======================== VARIABLES ==============================
		/*! Reference to database handling class. */
		DBInterface *archiveDBInterface;

		/*! Widget that holds the windows used to display archived spikes. */
		MonitorArea *monitorArea;
	
		/*! Manager that plays back the archive. */
		ArchiveManager *archiveManager;

		/*! Reference to main application used to prevent double access to 
			variables etc. */
		SpikeStreamApplication *spikeStrApp;

		/*! Used to ignore button events when changing button states programatically. */
		bool ignoreButton;

		/*! Vector of neuron groups in the loaded archive.*/
		vector<NeuronGroup*> neuronGrpVector;

		/*! Vector of the statistics being monitored for this archive.*/
		vector<ArchiveStatisticsHolder*> archiveStatisticsVector;

		/*! Used to give each ArchiveStatisticsHolder a unique id.*/
		unsigned int archiveStatisticsIDCount;

		/*! Holds a nicely formatted version of the network model.*/
		QString networkModelString;

		/*! Displays the XML text of the model.*/
		ViewModelDialog* viewModelDialog;

		//Qt Widgets for controls
		QPushButton *loadButton;
		QPushButton *unloadButton;
		QLineEdit *archInfoText;
		QPushButton *rewindButton;
		QPushButton *playButton;
		QPushButton *stepButton;
		QPushButton *fastForwardButton;
		QPushButton *stopButton;
		QLabel *frameRateLabel;
		QComboBox *frameRateCombo;
		QPushButton *dockAllButton;
		QPushButton *undockAllButton;
		QPixmap* editPixmap;

		//Qt widgets for statistics
		Q3GroupBox *statsGrpBox;
		QLabel *spikeNeurCountLabel_1;
		QLabel *spikeNeurCountLabel_2;
		QLabel *spikeNeurTotalLabel_1;
		QLabel *spikeNeurTotalLabel_2;
		Q3Table* statsTable;
		QPushButton *deleteStatsButton;

		//Location of columns in statistics table
		int checkBxCol;
		int editCol;
		int descCol;
		int neursPerTimeStepCol;
		int totalNeursCol;
		int idCol;

		//======================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ArchiveWidget(const ArchiveWidget&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ArchiveWidget operator = (const ArchiveWidget&);

		void addArchiveStatistics(ArchiveStatisticsHolder* archStatHold);
		void addArchiveStatistics(ArchiveStatisticsHolder* archStatHold, unsigned int archStatID);
		void deleteArchiveStatistics(unsigned int archStatID);
		void enableArchiveControls(bool);
		QString getDate(QString);
		void loadArchive(QString);
		void loadStatisticsTable();


};


#endif //ARCHIVEWIDGET_H


