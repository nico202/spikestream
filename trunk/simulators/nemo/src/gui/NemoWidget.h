#ifndef NEMOWIDGET_H
#define NEMOWIDGET_H

//SpikeStream includes
#include "NemoWrapper.h"

//Qt includes
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMutex>
#include <QProgressDialog>
#include <QPushButton>
#include <QToolBar>
#include <QWidget>

namespace spikestream {

	/*! Graphical interface for the plugin wrapping the Nemo spiking neural simulator */
	class NemoWidget : public QWidget {
		Q_OBJECT

		public:
			NemoWidget(QWidget* parent = 0);
			~NemoWidget();

		private slots:
			void archiveStateChanged(int state);
			void loadSimulation();
			void monitorStateChanged(int state);
			void nemoWrapperFinished();
			void networkChanged();
			void setArchiveDescription();
			void setNeuronParameters();
			void setNemoParameters();
			void setSynapseParameters();
			void simulationAdvanceOccurred(unsigned int timeStep);
			void simulationRateChanged(int comboIndex);
			void startSimulation();
			void stepSimulation();
			void stopSimulation();
			void unloadSimulation();
			void updateProgress(int stepsCompleted, int totalSteps);

		private:
			//========================  VARIABLES  ========================
			/*! Wrapper for Nemo library */
			NemoWrapper* nemoWrapper;

			/*! Box holding all widgets - makes it easy to enable and disable everything. */
			QGroupBox* mainGroupBox;

			/*! Button for loading simulation */
			QPushButton* loadButton;

			/*! Button for unloading simulation */
			QPushButton* unloadButton;

			/*! Button that launches dialog to edit the neuron parameters */
			QPushButton* neuronParametersButton;

			/*! Button that launches dialog to edit the synapse parameters */
			QPushButton* synapseParametersButton;

			/*! Button that launches dialog to edit the neuron parameters */
			QPushButton* nemoParametersButton;

			/*! Tool bar with transport controls for loading, playing, etc. */
			QToolBar* toolBar;

			/*! For user to enter a description of the archive */
			QLineEdit* archiveDescriptionEdit;

			/*! Sets the description of the archive */
			QPushButton* setArchiveDescriptionButton;

			/*! Rate of the simulation */
			QComboBox* simulationRateCombo;

			/*! Time step of the simulation */
			QLabel* timeStepLabel;

			/*! Dialog for giving feedback about progress */
			QProgressDialog* progressDialog;

			/*! Mutex for preventing multiple access to functions */
			QMutex mutex;

			/*! Records if task has been cancelled */
			bool taskCancelled;

			//=======================  METHODS  =========================
			void checkForErrors();
			void checkWidgetEnabled();
			QToolBar* getToolBar();
	};

}

#endif//NEMOWIDGET_H

