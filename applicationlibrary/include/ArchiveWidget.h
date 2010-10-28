#ifndef ARCHIVEWIDGET_H
#define ARCHIVEWIDGET_H

//SpikeStream includes
#include "ArchiveInfo.h"
#include "ArchivePlayerThread.h"
using namespace spikestream;

//Qt includes
#include <QAction>
#include <QToolBar>
#include <QWidget>
#include <QLayout>
#include <QHash>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>

namespace spikestream{

	class ArchiveWidget : public QWidget {
		Q_OBJECT

		public:
			ArchiveWidget(QWidget* parent=0);
			~ArchiveWidget();

		signals:
			void archiveChanged();

		private slots:
			void archivePlayerStopped();
			void deleteArchive();
			void loadArchive();
			void loadArchiveList();
			void rewindButtonPressed();
			void playButtonPressed();
			void stepButtonPressed();
			void fastForwardButtonPressed();
			void stopButtonPressed();
			void frameRateComboChanged(int);
			void setArchiveProperties();
			void updateTimeStep(unsigned timeStep, const QList<unsigned>& neuronIDList);


		private:
			//======================  VARIABLES  =====================
			/*! Keep the current list of archives in memory */
			QHash<unsigned int, ArchiveInfo> archiveInfoMap;

			/*! Layout used for organising widget. Need a reference to enable
			networks to be dynamically reloaded */
			QGridLayout* gridLayout;

			/*! Widget holding transport controls */
			QToolBar* toolBar;

			/*! Used to ignore button events when button state is changed programatically */
			bool ignoreButton;

			/*! Class responsible for playing the archive */
			ArchivePlayerThread* archivePlayer;

			/*! Records if the rewind button was used to stop the playback thread */
			bool rewind;

			/*! Records if the step button was used to stop the playback thread */
			bool step;

			/*! Records if any of the neuron firing patterns have been displayed.
				Important for the step function to display the first pattern in the archive */
			bool archiveOpen;

			/*! Action to play archive */
			QAction* playAction;

			/*! Action to stop playback of archive */
			QAction* stopAction;

			/*! Action to fast forward playback of archive */
			QAction* fastForwardAction;

			/*! Sets the rate of play back of the archive */
			QComboBox* frameRateCombo;

			/*! Displays the current time step */
			QLabel* timeStepLabel;

			/*! Displays the maximum time step */
			QLabel* maxTimeStepLabel;

			// Columns for different types of information
			const static int propCol = 0;
			const static int idCol = 1;
			const static int netIDCol = 2;
			const static int dateCol = 3;
			const static int descCol = 4;
			const static int loadButCol = 5;
			const static int delButCol = 6;
			const static int numCols = 7;

			//========================  METHODS  =========================
			QToolBar* getToolBar();
			void loadArchive(ArchiveInfo& archiveInfo);
			void reset();
			void rewindArchive();
			void setMaxTimeStepLabel();
			void stepArchive();

    };
}

#endif//ARCHIVEWIDGET_H

