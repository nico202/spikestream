//SpikeStream includes
#include "Globals.h"
#include "NemoParametersDialog.h"
#include "NemoWidget.h"
#include "NeuronParametersDialog.h"
#include "SpikeStreamException.h"
#include "SynapseParametersDialog.h"
using namespace spikestream;

//Qt includes
#include <QAction>
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
	//Create layout for the widget
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	mainGroupBox = new QGroupBox("Nemo CUDA Simulator", this);

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
	mainVBox->addWidget(toolBar);

	//Add widget to control live monitoring of simulation
	QCheckBox* monitorCheckBox = new QCheckBox("Monitor");
	connect(monitorCheckBox, SIGNAL(stateChanged(int)), this, SLOT(monitorStateChanged(int)));
	mainVBox->addWidget(monitorCheckBox);

	//Add widgets to control archiving
	QCheckBox* archiveCheckBox = new QCheckBox("Archive. Description: ");
	connect(archiveCheckBox, SIGNAL(stateChanged(int)), this, SLOT(archiveStateChanged(int)));
	archiveDescriptionEdit = new QLineEdit("Undescribed");
	setArchiveDescriptionButton = new QPushButton("Set");
	connect(setArchiveDescriptionButton, SIGNAL(clicked()), this, SLOT(setArchiveDescription()));
	QHBoxLayout* archiveLayout = new QHBoxLayout();
	archiveLayout->addWidget(archiveCheckBox);
	archiveLayout->addWidget(archiveDescriptionEdit);
	archiveLayout->addWidget(setArchiveDescriptionButton);
	mainVBox->addLayout(archiveLayout);

	//Put layout into enclosing box
	mainGroupBox->setLayout(mainVBox);
	this->setMinimumSize(800, 700);

	//Create wrapper for Nemo library
	nemoWrapper = new NemoWrapper();
	connect(nemoWrapper, SIGNAL(finished()), this, SLOT(nemoWrapperFinished()));
	connect(nemoWrapper, SIGNAL(progress(int,int)), this, SLOT(updateProgress(int, int)), Qt::QueuedConnection);

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

}


/*! Called when the nemo wrapper finishes a task it is engaged in */
void NemoWidget::nemoWrapperFinished(){
	checkForErrors();

	switch (nemoWrapper->getCurrentTask()){
		case NemoWrapper::LOAD_SIMULATION_TASK :
			if(!taskCancelled){
				loadButton->setEnabled(false);
				unloadButton->setEnabled(true);
			}
		break;
		case NemoWrapper::RUN_SIMULATION_TASK :
			qDebug()<<"NemoWidget: Simulation run finished";
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

	//Fix enabled status of toolbar
	checkWidgetEnabled();
}


/*! Sets the archive description using the contents of the archiveDescriptionEdit text field */
void NemoWidget::setArchiveDescription(){

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
void NemoWidget::simulationAdvanceOccurred(unsigned int timeStep){


	//Update the time step label
	//timeStepLabel->setText(nemoWrapper->getTimeStep());

}


/*! Called when the rate of the simulation has been changed by the user. */
void NemoWidget::simulationRateChanged(int comboIndex){

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
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Advances the simulation by one time step */
void NemoWidget::stepSimulation(){

}


/*! Stops the simulation */
void NemoWidget::stopSimulation(){
	nemoWrapper->stop();

}


/*! Instructs the nemo wrapper to discard the current simulation */
void NemoWidget::unloadSimulation(){
	//Double check that user wants to unload simulation
	int response = QMessageBox::warning(this, "Deleting Network", "Are you sure that you want to unload the simulation?", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
	if(response != QMessageBox::Ok)
		return;

	//Unload the simulation
	nemoWrapper->unload();

	//Set buttons appropriately
	loadButton->setEnabled(true);
	unloadButton->setEnabled(false);
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
void NemoWidget::checkForErrors(){
	if(nemoWrapper->isError()){
		qCritical()<<"NemoWrapper error: '"<<nemoWrapper->getErrorMessage()<<"'.";
	}
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

	QAction* tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/play.png"), "Start simulation", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(startSimulation()));
	//connect(Globals::getEventRouter(), SIGNAL(analysisNotRunningSignal(bool)), tmpAction, SLOT(setEnabled(bool)));
	tmpToolBar->addAction (tmpAction);

	tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/stop.png"), "Stop simulation", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(stopSimulation()));
	tmpToolBar->addAction (tmpAction);

	tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/step.png"), "Step simulation", this);
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

