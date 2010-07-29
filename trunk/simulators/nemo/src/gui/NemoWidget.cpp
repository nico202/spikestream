//SpikeStream includes
#include "Globals.h"
#include "NemoParametersDialog.h"
#include "NemoWidget.h"
#include "NeuronParametersDialog.h"
#include "SpikeStreamSimulationException.h"
#include "SynapseParametersDialog.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLayout>
#include <QMessageBox>
#include <QMutexLocker>


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
	//Register QList<unsigned> type to enable signals slots to work
	qRegisterMetaType< QList<unsigned> >("QList<unsigned>");

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

	//Add widget to control live monitoring of simulation
	monitorCheckBox = new QCheckBox("Monitor");
	connect(monitorCheckBox, SIGNAL(stateChanged(int)), this, SLOT(monitorStateChanged(int)));
	controlsVBox->addWidget(monitorCheckBox);

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

	//Put layout into enclosing box
	mainVBox->addWidget(controlsWidget);
	mainGroupBox->setLayout(mainVBox);
	this->setMinimumSize(800, 700);

	//Create wrapper for Nemo library
	nemoWrapper = new NemoWrapper();
	connect(nemoWrapper, SIGNAL(finished()), this, SLOT(nemoWrapperFinished()));
	connect(nemoWrapper, SIGNAL(progress(int,int)), this, SLOT(updateProgress(int, int)), Qt::QueuedConnection);
	connect(nemoWrapper, SIGNAL(timeStepChanged(unsigned, const QList<unsigned>&)), this, SLOT(updateTimeStep(unsigned, const QList<unsigned>&)), Qt::QueuedConnection);

	//Listen for network changes
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));

	//Set initial state of tool bar
	checkWidgetEnabled();

	//Initialise variables
	progressDialog = NULL;
}


/*! Destructor */
NemoWidget::~NemoWidget(){

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


/*! Instructs the Nemo wrapper to load the network from the database into Nemo */
void NemoWidget::loadSimulation(){
	if(!Globals::networkLoaded()){
		qCritical()<<"Cannot load simulation: no network loaded.";
		return;
	}
	if(nemoWrapper->isRunning()){
		qCritical()<<"Loading thread is already running - wait 10 seconds and try again.";
		return;
	}
	try{
		//Store the current neuron colour for monitoring
		neuronColor = Globals::getNetworkDisplay()->getFiringNeuronColor();

		//Start loading of simulation
		taskCancelled = false;
		nemoWrapper->prepareLoadSimulation();
		progressDialog = new QProgressDialog("Loading simulation", "Cancel", 0, 100, this);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(2000);
		nemoWrapper->start();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Switches the monitoring of the simulation on or off */
void NemoWidget::monitorStateChanged(int state){
	if(state == Qt::Checked)
		nemoWrapper->setMonitorMode(true);
	else{
		//Set the monitor mode
		nemoWrapper->setMonitorMode(false);

		//Clear highlights
		QHash<unsigned int, RGBColor*>* newHighlightMap = new QHash<unsigned int, RGBColor*>();
		Globals::getNetworkDisplay()->setNeuronColorMap(newHighlightMap);
	}
}


/*! Called when the nemo wrapper finishes a task it is engaged in */
void NemoWidget::nemoWrapperFinished(){
	//Handle errors
	if(checkForErrors()){
		switch (nemoWrapper->getCurrentTask()){
			case NemoWrapper::LOAD_SIMULATION_TASK :
				unloadSimulation(false);
			break;
			case NemoWrapper::RUN_SIMULATION_TASK :
				playAction->setEnabled(true);
				stopAction->setEnabled(false);
			break;
			case NemoWrapper::STEP_SIMULATION_TASK :
				playAction->setEnabled(true);
				stopAction->setEnabled(false);
			break;
			default :
				qCritical()<<"Nemo has finished executing, but task is not defined";
		}
		return;
	}

	//Finish off task
	switch (nemoWrapper->getCurrentTask()){
		case NemoWrapper::LOAD_SIMULATION_TASK :
			if(taskCancelled){
				unloadSimulation(false);
			}
			else{
				loadButton->setEnabled(false);
				unloadButton->setEnabled(true);
				neuronParametersButton->setEnabled(false);
				synapseParametersButton->setEnabled(false);
				nemoParametersButton->setEnabled(false);
				controlsWidget->setEnabled(true);
				Globals::setSimulationLoaded(true);
			}
		break;
		case NemoWrapper::RUN_SIMULATION_TASK :
			playAction->setEnabled(true);
			stopAction->setEnabled(false);
		break;
		case NemoWrapper::STEP_SIMULATION_TASK :
			playAction->setEnabled(true);
			stopAction->setEnabled(false);
		break;
		default :
			qCritical()<<"Nemo has finished executing, but task is not defined";
	}
}


/*! Called when the network is changed.
	Shows an error if this simulation is running, otherwise enables or disables the toolbar. */
void NemoWidget::networkChanged(){
	if(nemoWrapper->isRunning())
		qCritical()<<"Network should not be changed while simulation is running";

	//Unload simulation if it is loaded
	unloadSimulation(false);

	//Fix enabled status of toolbar
	checkWidgetEnabled();
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
		NemoParametersDialog dialog(nemoWrapper->getParameterInfoList(), nemoWrapper->getParameterValues(), nemoWrapper->getDefaultParameterValues(), this);
		if(dialog.exec() == QDialog::Accepted)
			nemoWrapper->setParameters(dialog.getParameters());
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


/*! Called when the simulation has advanced one time step */
void NemoWidget::updateTimeStep(unsigned int timeStep, const QList<unsigned>& neuronIDList){
	timeStepLabel->setText(QString::number(timeStep));

	if(nemoWrapper->isMonitorMode()){
		//Fill map with neuron ids
		QHash<unsigned int, RGBColor*>* newHighlightMap = new QHash<unsigned int, RGBColor*>();
		QList<unsigned>::const_iterator endList = neuronIDList.end();
		for(QList<unsigned>::const_iterator iter = neuronIDList.begin(); iter != endList; ++iter){
			(*newHighlightMap)[*iter] = neuronColor;
		}

		//Set network display
		Globals::getNetworkDisplay()->setNeuronColorMap(newHighlightMap);
	}

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
		nemoWrapper->prepareRunSimulation();
		nemoWrapper->start();
		playAction->setEnabled(false);
		stopAction->setEnabled(true);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Advances the simulation by one time step */
void NemoWidget::stepSimulation(){
	//Stop thread if it is running.
	if(nemoWrapper->isRunning()){
		nemoWrapper->stop();
	}

	//Simulation not running, so just step
	else{
		nemoWrapper->prepareStepSimulation();
		nemoWrapper->start();
		playAction->setEnabled(false);
		stopAction->setEnabled(true);
	}
}


/*! Stops the simulation */
void NemoWidget::stopSimulation(){
	nemoWrapper->stop();
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

	//Set buttons appropriately
	loadButton->setEnabled(true);
	unloadButton->setEnabled(false);
	neuronParametersButton->setEnabled(true);
	synapseParametersButton->setEnabled(true);
	nemoParametersButton->setEnabled(true);
	controlsWidget->setEnabled(false);
	archiveDescriptionEdit->setText("Undescribed");
	archiveCheckBox->setChecked(false);
	monitorCheckBox->setChecked(false);
	timeStepLabel->setText("0");
}


/*! Updates progress with a long operation */
void NemoWidget::updateProgress(int stepsCompleted, int totalSteps){
	//Protect code against multiple access
	QMutexLocker locker(&mutex);

	if(progressDialog == NULL)
		return;

	//Check for cancellation
	if(progressDialog->wasCanceled()){
		delete progressDialog;
		progressDialog = NULL;
		taskCancelled = true;
		nemoWrapper->stop();
	}

	//Update progress
	else if(stepsCompleted < totalSteps){
		progressDialog->setValue(stepsCompleted);
		progressDialog->setMaximum(totalSteps);
	}
	else if(stepsCompleted > totalSteps){
		qCritical()<<"Progress update error: Number of steps completed is greater than the number of possible steps.";
		return;
	}
	else{
		progressDialog->close();
		delete progressDialog;
		progressDialog = NULL;
	}
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
	timeStepLabel->setMinimumSize(50, 20);
	timeStepLabel->setMaximumSize(50, 20);
	timeStepLabel->setAlignment(Qt::AlignCenter);
	tmpToolBar->addWidget(timeStepLabel);

	return tmpToolBar;
}

