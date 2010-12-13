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
#include <QRadioButton>
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
			void checkResetWeightsProgress();
			void checkSaveWeightsProgress();
			void injectNoiseButtonClicked();
			void injectPatternButtonClicked();
			void loadPattern(QString comboStr);
			void loadSimulation();
			void monitorChanged(int state);
			void monitorNeuronsStateChanged(int monitorType);
			void nemoWrapperFinished();
			void networkChanged();
			void resetWeights();
			void setArchiveDescription();
			void setNeuronParameters();
			void setNemoParameters();
			void setSynapseParameters();
			void saveWeights();
			void setMonitorWeights(bool enable);
			void simulationRateChanged(int comboIndex);
			void simulationStopped();
			void startSimulation();
			void stepSimulation();
			void stopSimulation();
			void sustainNoiseChanged(bool enabled);
			void sustainPatternChanged(bool enabled);
			void unloadSimulation(bool confirmWithUser=true);
			void updateProgress(int stepsCompleted, int totalSteps);
			void updateTimeStep(unsigned int timeStep);
			void updateTimeStep(unsigned int timeStep, const QList<unsigned>& neuronIDList);
			void updateTimeStep(unsigned int timeStep, const QHash<unsigned, float>& membranePotentialMap);

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

			/*! Switches off monitoring of neurons */
			QRadioButton* noMonitorNeuronsButton;

			/*! Switches on monitoring of firing neurons. */
			QRadioButton* monitorFiringNeuronsButton;

			/*! Switches on monitoring of membrane potential */
			QRadioButton* monitorMemPotNeuronsButton;

			/*! Controls the monitoring of weights */
			QCheckBox* monitorWeightsCheckBox;

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

			/*! Button for resetting volatile weights */
			QPushButton* resetWeightsButton;

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

			/*! Timer to check on progress with heavy tasks with progress bar */
			QTimer* heavyTaskTimer;

			/*! Flag to prevent calls to progress dialog while it is redrawing. */
			bool updatingProgress;

			/*! Button to inject noise into a neuron group */
			QPushButton* injectNoiseButton;

			/*! Combo box controlling the percentage of injected noise */
			QComboBox* injectNoisePercentCombo;

			/*! Combo box controlling which neuron group noise is injected into */
			QComboBox* injectNoiseNeuronGroupCombo;

			/*! Combo to select which neuron group gets patterns injected into it. */
			QComboBox* injectPatternNeurGrpCombo;

			/*! Combo to select the pattern to be injected or to add a new pattern. */
			QComboBox* patternCombo;

			/*! Combo enabling user to switch between firing the neuron or setting an
				amount of injection current */
			QComboBox* patternCurrentCombo;

			/*! Button for injecting patterns into network. */
			QPushButton* injectPatternButton;

			/*! Map linking file path with patterns */
			QHash<QString, Pattern> patternMap;

			/*! Map with colours for plotting heat maps.
				indexes range from 0 to 10 inclusive with increasing temperature. */
			QHash<int, RGBColor*> heatColorMap;


			//=======================  METHODS  =========================
			bool checkForErrors();
			void checkWidgetEnabled();
			void createMembranePotentialColors();
			void fillPatternCurrentCombo();
			QString getFilePath(QString fileFilter);
			unsigned getNeuronGroupID(QString neurGrpStr);
			QString getPatternKey(const QString& patternComboText);
			QToolBar* getToolBar();
			void loadNeuronGroups();
			void setInjectNoise(bool sustain);
			void setInjectionPattern(bool sustain);
	};

}

#endif//NEMOWIDGET_H

