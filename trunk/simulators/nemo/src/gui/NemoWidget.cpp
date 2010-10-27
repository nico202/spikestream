//SpikeStream includes
#include "Globals.h"
#include "GlobalVariables.h"
#include "NemoParametersDialog.h"
#include "NemoWidget.h"
#include "NeuronParametersDialog.h"
#include "Pattern.h"
#include "PatternManager.h"
#include "SpikeStreamSimulationException.h"
#include "SynapseParametersDialog.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QButtonGroup>
#include <QDebug>
#include <QFileDialog>
#include <QLayout>
#include <QMessageBox>
#include <QMutexLocker>

/*! String user selects to load a pattern */
#define LOAD_PATTERN_STRING "Load Pattern"

#define MONITOR_NEURONS_OFF 0
#define MONITOR_NEURONS_FIRING 1
#define MONITOR_NEURONS_MEMBRANE 2

//Functions for dynamic library loading
extern "C" {
	/*! Creates a NemoWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new NemoWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Nemo CUDA Simulator");
	}
}


/*! Constructor */
NemoWidget::NemoWidget(QWidget* parent) : QWidget(parent) {
	//Register types to enable signals and slots to work
	qRegisterMetaType< QList<unsigned> >("QList<unsigned>");
	qRegisterMetaType< QHash<unsigned, float> >("QHash<unsigned, float>");

	//Create colours to be used for membrane potential
	createMembranePotentialColors();

	//Create layout for the entire widget
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	mainGroupBox = new QGroupBox("Nemo CUDA Simulator", this);
	controlsWidget = new QWidget();
	controlsWidget->setEnabled(false);
	QVBoxLayout* controlsVBox = new QVBoxLayout(controlsWidget);

	//Add load, unload and parameters buttons
	loadButton = new QPushButton("Load");
	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadSimulation()));
	unloadButton = new QPushButton("Unload");
	connect(unloadButton, SIGNAL(clicked()), this, SLOT(unloadSimulation()));
	unloadButton->setEnabled(false);
	neuronParametersButton = new QPushButton(" Neuron Parameters ");
	connect(neuronParametersButton, SIGNAL(clicked()), this, SLOT(setNeuronParameters()));
	synapseParametersButton = new QPushButton(" Synapse Parameters ");
	connect(synapseParametersButton, SIGNAL(clicked()), this, SLOT(setSynapseParameters()));
	nemoParametersButton = new QPushButton(" Nemo Parameters ");
	connect(nemoParametersButton, SIGNAL(clicked()), this, SLOT(setNemoParameters()));
	QHBoxLayout* loadLayout = new QHBoxLayout();
	loadLayout->addWidget(loadButton);
	loadLayout->addWidget(unloadButton);
	loadLayout->addWidget(neuronParametersButton);
	loadLayout->addWidget(synapseParametersButton);
	loadLayout->addWidget(nemoParametersButton);
	mainVBox->addLayout(loadLayout);

	//Add the tool bar
	toolBar = getToolBar();
	controlsVBox->addWidget(toolBar);

	//Add widget to control live monitoring of neurons
	QHBoxLayout* monitorNeuronsBox = new QHBoxLayout();
	monitorNeuronsBox->addWidget(new QLabel("Monitor neurons: "));
	QButtonGroup* monitorNeuronsButtonGroup = new QButtonGroup();
	connect(monitorNeuronsButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(monitorNeuronsStateChanged(int)));
	noMonitorNeuronsButton = new QRadioButton("Off");
	noMonitorNeuronsButton->setChecked(true);
	monitorNeuronsButtonGroup->addButton(noMonitorNeuronsButton, MONITOR_NEURONS_OFF);
	monitorNeuronsBox->addWidget(noMonitorNeuronsButton);
	monitorFiringNeuronsButton = new QRadioButton("Firing");
	monitorNeuronsButtonGroup->addButton(monitorFiringNeuronsButton, MONITOR_NEURONS_FIRING);
	monitorNeuronsBox->addWidget(monitorFiringNeuronsButton);
	monitorMemPotNeuronsButton = new QRadioButton("Membrane potential");
	monitorNeuronsButtonGroup->addButton(monitorMemPotNeuronsButton, MONITOR_NEURONS_MEMBRANE);
	monitorNeuronsBox->addWidget(monitorMemPotNeuronsButton);
	monitorNeuronsBox->addStretch(5);
	controlsVBox->addLayout(monitorNeuronsBox);

	//Add widgets to monitor, save and reset the weights
	QHBoxLayout* saveWeightsBox = new QHBoxLayout();
	monitorWeightsCheckBox = new QCheckBox("Monitor weights");
	connect(monitorWeightsCheckBox, SIGNAL(clicked(bool)), this, SLOT(setMonitorWeights(bool)));
	saveWeightsBox->addWidget(monitorWeightsCheckBox);
/*	resetWeightsButton = new QPushButton("Reset Weights");
	connect(resetWeightsButton, SIGNAL(clicked()), this, SLOT(resetWeights()));
	saveWeightsBox->addWidget(resetWeightsButton);*/
	saveWeightsButton = new QPushButton("Save Weights");
	connect(saveWeightsButton, SIGNAL(clicked()), this, SLOT(saveWeights()));
	saveWeightsBox->addWidget(saveWeightsButton);
	saveWeightsBox->addStretch(5);
	controlsVBox->addLayout(saveWeightsBox);

	//Add widgets to control archiving
	archiveCheckBox = new QCheckBox("Archive.");
	connect(archiveCheckBox, SIGNAL(stateChanged(int)), this, SLOT(archiveStateChanged(int)));
	archiveDescriptionEdit = new QLineEdit("Undescribed");
	archiveDescriptionEdit->setEnabled(false);
	setArchiveDescriptionButton = new QPushButton("Set Description");
	setArchiveDescriptionButton->setEnabled(false);
	connect(setArchiveDescriptionButton, SIGNAL(clicked()), this, SLOT(setArchiveDescription()));
	QHBoxLayout* archiveLayout = new QHBoxLayout();
	archiveLayout->addWidget(archiveCheckBox);
	archiveLayout->addWidget(archiveDescriptionEdit);
	archiveLayout->addWidget(setArchiveDescriptionButton);
	controlsVBox->addLayout(archiveLayout);

	//Add widgets to inject noise into specified layers
	QHBoxLayout* injectNoiseBox = new QHBoxLayout();
	injectNoiseNeuronGroupCombo = new QComboBox();
	injectNoiseNeuronGroupCombo->setMinimumSize(200, 20);
	injectNoiseBox->addWidget(injectNoiseNeuronGroupCombo);
	injectNoisePercentCombo = new QComboBox();
	for(int i=10; i<=100; i += 10)
		injectNoisePercentCombo->addItem(QString::number(i) + " %");
	injectNoisePercentCombo->setMinimumSize(60, 20);
	injectNoiseBox->addWidget(injectNoisePercentCombo);
	QPushButton* injectNoiseButton = new QPushButton("Inject Noise");
	injectNoiseButton->setMinimumHeight(20);
	connect(injectNoiseButton, SIGNAL(clicked()), this, SLOT(injectNoiseButtonClicked()));
	injectNoiseBox->addWidget(injectNoiseButton);
	injectNoiseBox->addStretch(5);
	controlsVBox->addSpacing(5);
	controlsVBox->addLayout(injectNoiseBox);

	//Add widgets to enable injection of patterns
	QHBoxLayout* injectPatternBox = new QHBoxLayout();
	injectPatternNeurGrpCombo = new QComboBox();
	injectPatternNeurGrpCombo->setMinimumSize(200, 20);
	injectPatternBox->addWidget(injectPatternNeurGrpCombo);
	patternCombo = new QComboBox();
	patternCombo->setMinimumSize(200, 20);
	patternCombo->addItem("");
	patternCombo->addItem(LOAD_PATTERN_STRING);
	connect(patternCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadPattern(QString)));
	injectPatternBox->addWidget(patternCombo);
	injectPatternButton = new QPushButton("Inject Pattern");
	injectPatternButton->setMinimumHeight(22);
	connect(injectPatternButton, SIGNAL(clicked()), this, SLOT(injectPatternButtonClicked()));
	injectPatternBox->addWidget(injectPatternButton);
	QCheckBox* sustainPatternChkBox = new QCheckBox("Sustain");
	connect(sustainPatternChkBox, SIGNAL(clicked(bool)), this, SLOT(sustainPatternChanged(bool)));
	injectPatternBox->addWidget(sustainPatternChkBox);
	controlsVBox->addSpacing(5);
	controlsVBox->addLayout(injectPatternBox);

	//Put layout into enclosing box
	mainVBox->addWidget(controlsWidget);
	mainGroupBox->setLayout(mainVBox);
	this->setMinimumSize(800, 700);

	//Create wrapper for Nemo library
	nemoWrapper = new NemoWrapper();
	connect(nemoWrapper, SIGNAL(finished()), this, SLOT(nemoWrapperFinished()));
	connect(nemoWrapper, SIGNAL(progress(int,int)), this, SLOT(updateProgress(int, int)), Qt::QueuedConnection);
	connect(nemoWrapper, SIGNAL(simulationStopped()), this, SLOT(simulationStopped()), Qt::QueuedConnection);
	connect(nemoWrapper, SIGNAL(timeStepChanged(unsigned)), this, SLOT(updateTimeStep(unsigned)), Qt::QueuedConnection);
	connect(nemoWrapper, SIGNAL(timeStepChanged(unsigned, const QList<unsigned>&)), this, SLOT(updateTimeStep(unsigned, const QList<unsigned>&)), Qt::QueuedConnection);
	connect(nemoWrapper, SIGNAL(timeStepChanged(unsigned, const QHash<unsigned, float>&)), this, SLOT(updateTimeStep(unsigned, const QHash<unsigned, float>&)), Qt::QueuedConnection);

	//Listen for network changes
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));

	//Set initial state of tool bar
	checkWidgetEnabled();

	//Initialise variables
	progressDialog = NULL;
}


/*! Destructor */
NemoWidget::~NemoWidget(){
	delete nemoWrapper;
	for(QHash<int, RGBColor*>::iterator iter = heatColorMap.begin(); iter != heatColorMap.end(); ++iter)
		delete iter.value();
	heatColorMap.clear();
}


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

/*! Switches the archiving of the simulation on or off */
void NemoWidget::archiveStateChanged(int state){
	if(state == Qt::Checked){
		archiveDescriptionEdit->setEnabled(true);
		setArchiveDescriptionButton->setEnabled(true);
		nemoWrapper->setArchiveMode(true);
	}
	else{
		archiveDescriptionEdit->setEnabled(false);
		setArchiveDescriptionButton->setEnabled(false);
		nemoWrapper->setArchiveMode(false);
	}
}


/*! Checks for progress with the loading. Has to be done with a timer because the thread is kept
	running ready for playing or stepping the simulation. Updates to the progress bar are done
	by the updateProgress method */
void NemoWidget::checkLoadingProgress(){
	//Check for errors during loading
	if(nemoWrapper->isError()){
		loadingTimer->stop();
		progressDialog->setValue(progressDialog->maximum());
		delete progressDialog;
		progressDialog = NULL;
		qCritical()<<"Error occurred loading simulation: '"<<nemoWrapper->getErrorMessage()<<"'.";
		return;
	}

	//Check for cancelation - stop timer and abort operation
	else if(progressDialog->wasCanceled()){
		loadingTimer->stop();
		nemoWrapper->cancelLoading();
		delete progressDialog;
		progressDialog = NULL;
		return;
	}

	//If simulation has not been loaded return with loading timer still running
	else if(!nemoWrapper->isSimulationLoaded()){
		return;
	}

	else if (updatingProgress){
		return;
	}

	//If we have reached this point, loading is complete
	loadingTimer->stop();
	progressDialog->setValue(progressDialog->maximum());
	delete progressDialog;
	progressDialog = NULL;

	//Adjust buttons
	loadButton->setEnabled(false);
	unloadButton->setEnabled(true);
	synapseParametersButton->setEnabled(false);
	nemoParametersButton->setEnabled(false);
	controlsWidget->setEnabled(true);
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	monitorWeightsCheckBox->setChecked(nemoWrapper->isMonitorWeights());

	//Cannot save weights or archive if the network is not fully saved in the database
	if(!Globals::getNetwork()->isSaved()){
		saveWeightsButton->setEnabled(false);
		archiveCheckBox->setEnabled(false);
		setArchiveDescriptionButton->setEnabled(false);
		archiveDescriptionEdit->setEnabled(false);
	}
	else{
		saveWeightsButton->setEnabled(true);
		archiveCheckBox->setEnabled(true);
		setArchiveDescriptionButton->setEnabled(true);
		archiveDescriptionEdit->setEnabled(true);
	}

	//Set nemo wrapper as the simulation in global scope
	Globals::setSimulation(nemoWrapper);
}


/*! Checks for progress with the resetting of weights*/
void NemoWidget::checkResetWeightsProgress(){
	//Check for errors during resetting weights
	if(nemoWrapper->isError()){
		heavyTaskTimer->stop();
		progressDialog->setValue(progressDialog->maximum());
		delete progressDialog;
		progressDialog = NULL;
		qCritical()<<"Error occurred resetting of weights: '"<<nemoWrapper->getErrorMessage()<<"'.";
		return;
	}

	//Check for cancelation - stop timer and abort operation
	else if(progressDialog->wasCanceled()){
		heavyTaskTimer->stop();
		nemoWrapper->cancelResetWeights();
		delete progressDialog;
		progressDialog = NULL;
		return;
	}

	//If save weights is not complete return with timer running
	else if(!nemoWrapper->isWeightsReset()){
		return;
	}

	else if (updatingProgress){
		return;
	}

	//If we have reached this point, loading is complete
	heavyTaskTimer->stop();
	progressDialog->setValue(progressDialog->maximum());
	delete progressDialog;
	progressDialog = NULL;
}


/*! Checks for progress with the saving of weights*/
void NemoWidget::checkSaveWeightsProgress(){
	//Check for errors during loading
	if(nemoWrapper->isError()){
		heavyTaskTimer->stop();
		progressDialog->setValue(progressDialog->maximum());
		delete progressDialog;
		progressDialog = NULL;
		qCritical()<<"Error occurred saving of weights: '"<<nemoWrapper->getErrorMessage()<<"'.";
		return;
	}

	//Check for cancelation - stop timer and abort operation
	else if(progressDialog->wasCanceled()){
		heavyTaskTimer->stop();
		nemoWrapper->cancelSaveWeights();
		delete progressDialog;
		progressDialog = NULL;
		return;
	}

	//If simulation has not been loaded return with loading timer still running
	else if(!nemoWrapper->isWeightsSaved()){
		return;
	}

	else if (updatingProgress){
		return;
	}

	//If we have reached this point, loading is complete
	heavyTaskTimer->stop();
	progressDialog->setValue(progressDialog->maximum());
	delete progressDialog;
	progressDialog = NULL;
}


/*! Called when the inject noise button is clicked. Sets the injection
	of noise in the nemo wrapper */
void NemoWidget::injectNoiseButtonClicked(){
	unsigned percentage = Util::getUInt(injectNoisePercentCombo->currentText().section(" ", 0, 0));
	unsigned neurGrpID = getNeuronGroupID(injectNoiseNeuronGroupCombo->currentText());
	try{
		nemoWrapper->setInjectNoise(neurGrpID, percentage);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Called when inject pattern button is clicked.
	Sets the injection of the selected pattern for the next time step in the nemo wrapper */
void NemoWidget::injectPatternButtonClicked(){
	//Set pattern in the nemo wrapper
	try{
		nemoWrapper->setInjectionPattern(patternMap[getPatternKey(patternCombo->currentText())], getNeuronGroupID(injectPatternNeurGrpCombo->currentText()), false);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! If the string is set to LOAD_PATTERN_STRING, a dialog is displayed
	for the user to load up a new pattern into memory. */
void NemoWidget::loadPattern(QString comboStr){
	//Return if we are setting the pattern to another pattern.
	if(comboStr != LOAD_PATTERN_STRING){
		return;
	}

	//Reset pattern combo
	patternCombo->setCurrentIndex(0);

	//Select file containing new pattern.
	QString filePath = getFilePath("*.pat");
	if(filePath.isEmpty())
		return;
	if(!QFile::exists(filePath)){
		qCritical()<<"Selected file '"<<filePath<<"' does not exist.";
		return;
	}

	try{
		//Load up the new pattern
		Pattern newPattern;
		PatternManager::load(filePath, newPattern);

		//Add name to combo if it does not already exist
		if(!patternMap.contains(filePath))
			patternCombo->insertItem(patternCombo->count() - 1, newPattern.getName());

		//Replace pattern stored for this name with new pattern or add it if it doesn't exist.
		patternMap[filePath] = newPattern;

		//Set combo to display loaded item
		if(patternCombo->itemText(0) == "")
			patternCombo->removeItem(0);
		patternCombo->setCurrentIndex(patternCombo->count() - 2);
	}
	catch(SpikeStreamException* ex){
		qCritical()<<ex->getMessage();
	}
}


/*! Instructs the Nemo wrapper to load the network from the database into Nemo */
void NemoWidget::loadSimulation(){
	//Run some checks
	if(!Globals::networkLoaded()){
		qCritical()<<"Cannot load simulation: no network loaded.";
		return;
	}
	if(nemoWrapper->isRunning()){
		qCritical()<<"Nemo wrapper is already running - cannot load simulation.";
		return;
	}
	if(nemoWrapper->isSimulationLoaded()){
		qCritical()<<"Simulation is already loaded - you must unload the current simulation before loading another.";
		return;
	}

	//Load the current neuron groups into control widgets
	loadNeuronGroups();

	try{
		//Store the current neuron colour for monitoring
		neuronColor = Globals::getNetworkDisplay()->getSimulationFiringNeuronColor();

		//Start loading of simulation
		taskCancelled = false;
		progressDialog = new QProgressDialog("Loading simulation", "Cancel", 0, 100, this);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(1000);
		updatingProgress = false;
		nemoWrapper->start();//Load carried out by run method

		//Wait for loading to finish and update progress dialog
		loadingTimer  = new QTimer(this);
		connect(loadingTimer, SIGNAL(timeout()), this, SLOT(checkLoadingProgress()));
		loadingTimer->start(200);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Controls whether the time step is updated each time step. */
void NemoWidget::monitorChanged(int state){
	try{
		if(state == Qt::Checked){
			nemoWrapper->setMonitor(true);
			if(nemoWrapper->isSimulationLoaded()){
				timeStepLabel->setText(QString::number(nemoWrapper->getTimeStep()));
			}
			noMonitorNeuronsButton->setEnabled(true);
			monitorFiringNeuronsButton->setEnabled(true);
			monitorMemPotNeuronsButton->setEnabled(true);
			monitorWeightsCheckBox->setEnabled(true);
		}
		else{
			nemoWrapper->setMonitor(false);
			noMonitorNeuronsButton->setEnabled(false);
			monitorFiringNeuronsButton->setEnabled(false);
			monitorMemPotNeuronsButton->setEnabled(false);
			monitorWeightsCheckBox->setEnabled(false);
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Switches the monitoring of the simulation on or off */
void NemoWidget::monitorNeuronsStateChanged(int monitorType){
	try{
		//No monitoring at all
		if(monitorType == MONITOR_NEURONS_OFF)
			nemoWrapper->setMonitorNeurons(false, false);
		//Monitoring firing neurons
		else if(monitorType == MONITOR_NEURONS_FIRING){
			nemoWrapper->setMonitorNeurons(true, false);
		}
		//Monitoring membrane potential
		else if(monitorType == MONITOR_NEURONS_MEMBRANE){
			nemoWrapper->setMonitorNeurons(false, true);
		}
		//Unrecognized value
		else{
			throw SpikeStreamException("Monitor neuron type not recognized: " + QString::number(monitorType));
		}

		//Clear current highlights
		QHash<unsigned int, RGBColor*>* newHighlightMap = new QHash<unsigned int, RGBColor*>();
		Globals::getNetworkDisplay()->setNeuronColorMap(newHighlightMap);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Called when the nemo wrapper thread exits.
	Unloads simulation and resets everything. */
void NemoWidget::nemoWrapperFinished(){
	checkForErrors();
	unloadSimulation(false);
}


/*! Called when the network is changed.
	Shows an error if this simulation is running, otherwise enables or disables the toolbar. */
void NemoWidget::networkChanged(){
	if(nemoWrapper->isSimulationRunning())
		qCritical()<<"Network should not be changed while simulation is running";

	//Unload simulation if it is loaded
	unloadSimulation(false);

	//Fix enabled status of toolbar
	checkWidgetEnabled();
}


/*! Instructs NeMo wrapper to reset weights to
	stored values at the next time step */
void NemoWidget::resetWeights(){
	//Check user wants to save weights.
	int response = QMessageBox::warning(this, "Reset Weights?", "Are you sure that you want to reset the weights.\nThis will overwrite the trained weights and cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if(response != QMessageBox::Ok)
		return;

	try{

		//Start resetting of weights
		updatingProgress = false;
		taskCancelled = false;
		progressDialog = new QProgressDialog("Resetting weights", "Cancel", 0, 100, this);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(1000);

		//Instruct wrapper thread to start saving weights
		nemoWrapper->resetWeights();

		//Wait for loading to finish and update progress dialog
		heavyTaskTimer  = new QTimer(this);
		connect(heavyTaskTimer, SIGNAL(timeout()), this, SLOT(checkResetWeightsProgress()));
		heavyTaskTimer->start(200);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}

/*! Instructs NeMo wrapper to save weights to
	weight field in database at the next time step */
void NemoWidget::saveWeights(){
	//Check user wants to save weights.
	int response = QMessageBox::warning(this, "Save Weights?", "Are you sure that you want to save weights.\nThis will overwrite the current weights in the database and cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if(response != QMessageBox::Ok)
		return;

	//Double check that network does not have analyses
	if(Globals::getAnalysisDao()->networkHasAnalyses(Globals::getNetwork()->getID())){
		qCritical()<<"Network is linked to analyses - weights cannot be saved until analyses are deleted.";
		return;
	}
	try{

		//Start saving of weights
		updatingProgress = false;
		taskCancelled = false;
		progressDialog = new QProgressDialog("Saving weights", "Cancel", 0, 100, this);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(1000);

		//Instruct wrapper thread to start saving weights
		nemoWrapper->saveWeights();

		//Wait for loading to finish and update progress dialog
		heavyTaskTimer  = new QTimer(this);
		connect(heavyTaskTimer, SIGNAL(timeout()), this, SLOT(checkSaveWeightsProgress()));
		heavyTaskTimer->start(200);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Instructs NeMo wrapper to monitor weights by saving them to
	 temporary weight field in network at each time step */
void NemoWidget::setMonitorWeights(bool enable){
	nemoWrapper->setMonitorWeights(enable);
}


/*! Sets the archive description using the contents of the archiveDescriptionEdit text field */
void NemoWidget::setArchiveDescription(){
	if(archiveDescriptionEdit->text().isEmpty())
		archiveDescriptionEdit->setText("Undescribed");
	if(nemoWrapper->getArchiveID() == 0)
		throw SpikeStreamSimulationException("Attempting to set archive description when no archive is loaded.");
	Globals::getArchiveDao()->setArchiveDescription(nemoWrapper->getArchiveID(), archiveDescriptionEdit->text());
	Globals::getEventRouter()->archiveListChangedSlot();
}


/*! Sets the parameters of the neurons in the network */
void  NemoWidget::setNeuronParameters(){
	NeuronParametersDialog* dialog = new NeuronParametersDialog(this);
	dialog->exec();
	delete dialog;
}


/*! Sets the parameters of Nemo */
void NemoWidget::setNemoParameters(){
	try{
		NemoParametersDialog dialog(nemoWrapper->getNemoConfig(), nemoWrapper->getSTDPFunctionID(), this);
		if(dialog.exec() == QDialog::Accepted){
			nemoWrapper->setNemoConfig(dialog.getNemoConfig());
			nemoWrapper->setSTDPFunctionID(dialog.getSTDPFunctionID());
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Sets the parameters of the synapses in the network */
void NemoWidget::setSynapseParameters(){
	SynapseParametersDialog* dialog = new SynapseParametersDialog(this);
	dialog->exec();
	delete dialog;
}


/*! Called when NeMo stops playing */
void NemoWidget::simulationStopped(){
	saveWeightsButton->setEnabled(true);
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	unloadButton->setEnabled(true);
	neuronParametersButton->setEnabled(true);
}


/*! Called when the simulation has advanced one time step
	This version of the method only updates the time step. */
void NemoWidget::updateTimeStep(unsigned int timeStep){
	timeStepLabel->setText(QString::number(timeStep));

	//Allow simulation to proceed on to next step
	nemoWrapper->clearWaitForGraphics();
}


/*! Called when the simulation has advanced one time step.
	This version fo the method updates the time step and firing neuron IDs. */
void NemoWidget::updateTimeStep(unsigned int timeStep, const QList<unsigned>& neuronIDList){
	timeStepLabel->setText(QString::number(timeStep));

	//Fill map with neuron ids
	QHash<unsigned int, RGBColor*>* newHighlightMap = new QHash<unsigned int, RGBColor*>();
	QList<unsigned>::const_iterator endList = neuronIDList.end();
	for(QList<unsigned>::const_iterator iter = neuronIDList.begin(); iter != endList; ++iter){
		(*newHighlightMap)[*iter] = neuronColor;
	}

	//Set network display
	Globals::getNetworkDisplay()->setNeuronColorMap(newHighlightMap);

	//Allow simulation to proceed on to next step
	nemoWrapper->clearWaitForGraphics();
}


/*! Called when the simulation has advanced one time step.
	This version of the method updats the time step and membrane potential. */
void NemoWidget::updateTimeStep(unsigned int timeStep, const QHash<unsigned, float>& membranePotentialMap){
	timeStepLabel->setText(QString::number(timeStep));

	//Fill map with appropriate colours depending on membrane potential.
	float tmpMemPot = 0.0f;
	QHash<unsigned int, RGBColor*>* newHighlightMap = new QHash<unsigned int, RGBColor*>();
	QHash<unsigned, float>::const_iterator endMap = membranePotentialMap.end();
	for(QHash<unsigned, float>::const_iterator iter = membranePotentialMap.begin(); iter != endMap; ++iter){
		tmpMemPot = iter.value();
		if(tmpMemPot < -89.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[-1];
		else if(tmpMemPot < -78.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[0];
		else if(tmpMemPot < -67.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[1];
		else if(tmpMemPot < -56.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[2];
		else if(tmpMemPot < -45.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[3];
		else if(tmpMemPot < -34.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[4];
		else if(tmpMemPot < -23.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[5];
		else if(tmpMemPot < -12.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[6];
		else if(tmpMemPot < -1.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[7];
		else if(tmpMemPot < 10.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[8];
		else if(tmpMemPot < 21.0f)
			(*newHighlightMap)[iter.key()] = heatColorMap[9];
		else
			(*newHighlightMap)[iter.key()] = heatColorMap[10];
	}

	//Set network display
	Globals::getNetworkDisplay()->setNeuronColorMap(newHighlightMap);

	//Allow simulation to proceed on to next step
	nemoWrapper->clearWaitForGraphics();
}


/*! Called when the rate of the simulation has been changed by the user. */
void NemoWidget::simulationRateChanged(int){
	if(simulationRateCombo->currentText() == "Max")
		nemoWrapper->setFrameRate(0);
	else{
		unsigned int frameRate = Util::getUInt(simulationRateCombo->currentText());
		nemoWrapper->setFrameRate(frameRate);
	}
}


/*! Starts the simulation running */
void NemoWidget::startSimulation(){
	if(!nemoWrapper->isSimulationLoaded()){
		qCritical()<<"Cannot play simulation: no simulation loaded.";
		return;
	}
	try{
		nemoWrapper->playSimulation();
		playAction->setEnabled(false);
		stopAction->setEnabled(true);
		unloadButton->setEnabled(false);
		neuronParametersButton->setEnabled(false);
		saveWeightsButton->setEnabled(false);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Advances the simulation by one time step */
void NemoWidget::stepSimulation(){
	if(!nemoWrapper->isSimulationLoaded()){
		qCritical()<<"Cannot step simulation: no simulation loaded.";
		return;
	}

	//Step simulation
	try{
		nemoWrapper->stepSimulation();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Stops the simulation */
void NemoWidget::stopSimulation(){
	nemoWrapper->stopSimulation();
}


/*! Switches between pattern injection controlled by button and
	continuous pattern injection at every time step */
void NemoWidget::sustainPatternChanged(bool enabled){
	if(enabled){
		injectPatternButton->setEnabled(false);
		injectPatternNeurGrpCombo->setEnabled(false);
		patternCombo->setEnabled(false);

		//Set pattern in the nemo wrapper
		try{
			nemoWrapper->setInjectionPattern(patternMap[getPatternKey(patternCombo->currentText())], getNeuronGroupID(injectPatternNeurGrpCombo->currentText()), true);
		}
		catch(SpikeStreamException& ex){
			qCritical()<<ex.getMessage();
		}
	}
	else{
		injectPatternButton->setEnabled(true);
		injectPatternNeurGrpCombo->setEnabled(true);
		patternCombo->setEnabled(true);

		//Switch off sustain pattern - the pattern will be automatically deleted on next step.
		nemoWrapper->setSustainPattern(false);
	}
}


/*! Instructs the nemo wrapper to discard the current simulation */
void NemoWidget::unloadSimulation(bool confirmWithUser){
	//Double check that user wants to unload simulation
	if(confirmWithUser){
		int response = QMessageBox::warning(this, "Unload?", "Are you sure that you want to unload the simulation?", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if(response != QMessageBox::Ok)
			return;
	}

	//Unload the simulation
	nemoWrapper->unloadSimulation();
	Globals::setSimulation(NULL);

	//Clear network display
	Globals::getNetworkDisplay()->setNeuronColorMap(new QHash<unsigned int, RGBColor*>());

	//Set buttons appropriately
	loadButton->setEnabled(true);
	unloadButton->setEnabled(false);
	synapseParametersButton->setEnabled(true);
	nemoParametersButton->setEnabled(true);
	controlsWidget->setEnabled(false);
	archiveDescriptionEdit->setText("Undescribed");
	timeStepLabel->setText("0");
}


/*! Updates progress with loading the simulation */
void NemoWidget::updateProgress(int stepsCompleted, int totalSteps){
	//Set flag to avoid multiple calls to progress dialog while it is redrawing
	if(updatingProgress)
		return;
	updatingProgress = true;

	if(progressDialog == NULL)
		return;

	//Check numbers are sensible
	if(stepsCompleted > totalSteps){
		qCritical()<<"Progress update error: Number of steps completed is greater than the number of possible steps.";
		return;
	}

	//Update progress
	if(stepsCompleted < totalSteps){
		progressDialog->setValue(stepsCompleted);
		progressDialog->setMaximum(totalSteps);
	}

	//Clear updating progress flag
	updatingProgress = false;
}


/*----------------------------------------------------------*/
/*------               PRIVATE METHODS                ------*/
/*----------------------------------------------------------*/

/*! Checks the nemo wrapper for errors */
bool NemoWidget::checkForErrors(){
	if(nemoWrapper->isError()){
		qCritical()<<"NemoWrapper error: '"<<nemoWrapper->getErrorMessage()<<"'.";
		return true;
	}
	return false;
}


/*! Checks to see if network is been loaded and sets enabled status of toolbar appropriately */
void NemoWidget::checkWidgetEnabled(){
	if(Globals::networkLoaded())
		mainGroupBox->setEnabled(true);
	else
		mainGroupBox->setEnabled(false);
}


/*! Builds a set of colours for use when displaying the membrane potential. */
void NemoWidget::createMembranePotentialColors(){
		heatColorMap[-1] = new RGBColor(0.0f, 0.0f, 0.0f);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[-1]);
		heatColorMap[0] = new RGBColor(HEAT_COLOR_0);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[0]);
		heatColorMap[1] = new RGBColor(HEAT_COLOR_1);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[1]);
		heatColorMap[2] = new RGBColor(HEAT_COLOR_2);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[2]);
		heatColorMap[3] = new RGBColor(HEAT_COLOR_3);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[3]);
		heatColorMap[4] = new RGBColor(HEAT_COLOR_4);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[4]);
		heatColorMap[5] = new RGBColor(HEAT_COLOR_5);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[5]);
		heatColorMap[6] = new RGBColor(HEAT_COLOR_6);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[6]);
		heatColorMap[7] = new RGBColor(HEAT_COLOR_7);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[7]);
		heatColorMap[8] = new RGBColor(HEAT_COLOR_8);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[8]);
		heatColorMap[9] = new RGBColor(HEAT_COLOR_9);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[9]);
		heatColorMap[10] = new RGBColor(HEAT_COLOR_10);
		Globals::getNetworkDisplay()->addDefaultColor(heatColorMap[10]);
}


/*! Enables user to enter a file path */
QString NemoWidget::getFilePath(QString fileFilter){
	QFileDialog dialog(this);
	dialog.setDirectory(Globals::getWorkingDirectory());
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter( QString("Configuration files (" + fileFilter + ")") );
	dialog.setViewMode(QFileDialog::Detail);
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	if(fileNames.size() > 0)
		return fileNames[0];
	else
		return QString("");
}


/*! Extracts the neuron group id from the supplied string. */
unsigned NemoWidget::getNeuronGroupID(QString neurGrpStr){
	QRegExp regExp("[()]");
	return Util::getUInt(neurGrpStr.section(regExp, 1, 1));
}


/*! The pattern map uses the file path as the key; the pattern combo uses the name of the pattern.
	This method looks up the file path corresponding to a pattern name, returning the first result
	that is found. Exception is thrown if pattern name does not exist in the patterns in the pattern
	map. */
QString NemoWidget::getPatternKey(const QString& patternComboText){
	if(patternComboText == LOAD_PATTERN_STRING){
		throw SpikeStreamException("Cannot inject pattern: no patterns added.");
	}

	//Find the key to the pattern in the pattern map
	QString patternKey = "";
	for(QHash<QString, Pattern>::iterator iter = patternMap.begin(); iter != patternMap.end(); ++iter){
		if(iter.value().getName() == patternComboText){
			patternKey = iter.key();
			break;
		}
	}

	//Check that key has been found
	if(patternKey.isEmpty()){
		throw SpikeStreamException("Pattern not found: " + patternComboText);
	}

	return patternKey;
}


/*! Builds the toolbar that goes at the top of the page. */
QToolBar* NemoWidget::getToolBar(){
	QToolBar* tmpToolBar = new QToolBar(this);

	playAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/play.png"), "Start simulation", this);
	connect(playAction, SIGNAL(triggered()), this, SLOT(startSimulation()));
	tmpToolBar->addAction (playAction);

	stopAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/stop.png"), "Stop simulation", this);
	connect(stopAction, SIGNAL(triggered()), this, SLOT(stopSimulation()));
	stopAction->setEnabled(false);
	tmpToolBar->addAction (stopAction);

	QAction* tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/step.png"), "Step simulation", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(stepSimulation()));

	tmpToolBar->addAction (tmpAction);
	simulationRateCombo = new QComboBox();
	simulationRateCombo->addItem("1");
	simulationRateCombo->addItem("5");
	simulationRateCombo->addItem("10");
	simulationRateCombo->addItem("15");
	simulationRateCombo->addItem("20");
	simulationRateCombo->addItem("25");
	simulationRateCombo->addItem("Max");
	connect(simulationRateCombo, SIGNAL(activated(int)), this, SLOT(simulationRateChanged(int)));
	tmpToolBar->addWidget(simulationRateCombo);

	timeStepLabel = new QLabel ("0");
	timeStepLabel->setStyleSheet( "QLabel { margin-left: 5px; background-color: #ffffff; border-color: #555555; border-width: 2px; border-style: outset; font-weight: bold;}");
	timeStepLabel->setMinimumSize(100, 20);
	timeStepLabel->setMaximumSize(100, 20);
	timeStepLabel->setAlignment(Qt::AlignCenter);
	tmpToolBar->addWidget(timeStepLabel);

	QCheckBox* monitorChkBox = new QCheckBox("Monitor");
	monitorChkBox->setChecked(true);
	connect(monitorChkBox, SIGNAL(stateChanged(int)), this, SLOT(monitorChanged(int)));
	tmpToolBar->addSeparator();
	tmpToolBar->addWidget(monitorChkBox);

	return tmpToolBar;
}

/*! Loads the current neuron groups into the control widgets */
void NemoWidget::loadNeuronGroups(){
	injectNoiseNeuronGroupCombo->clear();
	injectPatternNeurGrpCombo->clear();

	QList<NeuronGroupInfo> neurGrpInfoList = Globals::getNetwork()->getNeuronGroupsInfo();
	foreach(NeuronGroupInfo info, neurGrpInfoList){
		injectNoiseNeuronGroupCombo->addItem(info.getName() + "(" + QString::number(info.getID()) + ")");
		injectPatternNeurGrpCombo->addItem(info.getName() + "(" + QString::number(info.getID()) + ")");
	}
}


