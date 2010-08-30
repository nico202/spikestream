#ifndef NEMOWIDGET_H
#define NEMOWIDGET_H

//SpikeStream includes
#include "NemoWrapper.h"
#include "RGBColor.h"

//Qt includes
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMutex>
#include <QProgressDialog>
#include <QPushButton>
#include <QTimer>
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
			void checkLoadingProgress();
			void checkSaveWeightsProgress();
			void injectNoiseButtonClicked();
			void loadSimulation();
			void monitorStateChanged(int state);
			void nemoWrapperFinished();
			void networkChanged();
			void setArchiveDescription();
			void setNeuronParameters();
			void setNemoParameters();
			void setSynapseParameters();
			void updateTimeStep(unsigned int timeStep, const QList<unsigned>& neuronIDList);
			void saveWeights();
			void setTrackWeights(bool enable);
			void simulationRateChanged(int comboIndex);
			void simulationStopped();
			void startSimulation();
			void stepSimulation();
			void stopSimulation();
			void unloadSimulation(bool confirmWithUser=true);
			void updateProgress(int stepsCompleted, int totalSteps);

		private:
			//========================  VARIABLES  ========================
			/*! Wrapper for Nemo library */
			NemoWrapper* nemoWrapper;

			/*! Box holding all widgets - makes it easy to enable and disable everything. */
			QGroupBox* mainGroupBox;

			/*! Box holding widgets that need to be enabled when simulation is loaded */
			QWidget* controlsWidget;

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

			/*! Play action */
			QAction* playAction;

			/*! Stop action */
			QAction* stopAction;

			/*! Sets the simulation into monitor mode */
			QCheckBox* monitorCheckBox;

			/*! Sets the simulation into archive mode */
			QCheckBox* archiveCheckBox;

			/*! For user to enter a description of the archive */
			QLineEdit* archiveDescriptionEdit;

			/*! Sets the description of the archive */
			QPushButton* setArchiveDescriptionButton;

			/*! Rate of the simulation */
			QComboBox* simulationRateCombo;

			/*! Time step of the simulation */
			QLabel* timeStepLabel;

			/*! Button for saving volatile weights */
			QPushButton* saveWeightsButton;

			/*! Dialog for giving feedback about progress */
			QProgressDialog* progressDialog;

			/*! Mutex for preventing multiple access to functions */
			QMutex mutex;

			/*! Records if task has been cancelled */
			bool taskCancelled;

			/*! The colour of neurons that are currently firing */
			RGBColor* neuronColor;

			/*! Timer to check on loading progress */
			QTimer* loadingTimer;

			/*! Timer to check on progress with saving weights*/
			QTimer* saveWeightsTimer;

			/*! Flag to prevent calls to progress dialog while it is redrawing. */
			bool updatingProgress;

			/*! Combo box controlling the percentage of injected noise */
			QComboBox* injectNoisePercentCombo;

			/*! Combo box controlling which neuron group noise is injected into */
			QComboBox* injectNoiseNeuronGroupCombo;

			//=======================  METHODS  =========================
			bool checkForErrors();
			void checkWidgetEnabled();
			unsigned getNeuronGroupID(QString neurGrpStr);
			QToolBar* getToolBar();
			void loadNeuronGroups();
	};

}

#endif//NEMOWIDGET_H

