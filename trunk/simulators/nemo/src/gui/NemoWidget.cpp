//SpikeStream includes
#include "Globals.h"
#include "NemoWidget.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QAction>
#include <QDebug>
#include <QLayout>


//Functions for dynamic library loading
extern "C" {
	/*! Creates a Random1Widget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new NemoWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Nemo CUDA Simulator");
	}
}


/*! Constructor */
NemoWidget::NemoWidget(QWidget* parent) : QWidget() {
	//Create layout for the widget
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	//Add load, unload and parameters buttons
	loadButton = new QPushButton("Load");
	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadSimulation()));
	unloadButton = new QPushButton("Unload");
	connect(unloadButton, SIGNAL(clicked()), this, SLOT(unloadSimulation()));
	parametersButton = new QPushButton("Parameters");
	connect(parametersButton, SIGNAL(clicked()), this, SLOT(setParameters()));
	QHBoxLayout* loadLayout = new QHBoxLayout();
	loadLayout->addWidget(loadButton);
	loadLayout->addWidget(unloadButton);
	loadLayout->addWidget(parametersButton);
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

	//Create wrapper for Nemo library
	connect(nemoWrapper, SIGNAL(finished()), this, SLOT(nemoWrapperFinished()));

}


/*! Destructor */
NemoWidget::~NemoWidget(){

}


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

/*! Instructs the Nemo wrapper to load the network from the database into Nemo */
void NemoWidget::loadSimulation(){
	if(!Globals::networkLoaded()){
		qCritical()<<"Cannot load simulation: no network loaded.";
		return;
	}
	try{
		nemoWrapper->prepareLoadSimulation();
		nemoWrapper->start();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Called when the nemo wrapper finishes a task it is engaged in */
void NemoWidget::nemoFinished(){
	checkForErrors();

	switch (nemoWrapper->getCurrentTask()){
		case NemoWrapper::LOAD_SIMULATION_TASK :
			qDebug()<<"NemoWidget: Load simulation finished";
		break;
		case NemoWrapper::PLAY_SIMULATION_TASK :
			qDebug()<<"NemoWidget: Play simulation finished";
		break;
		default :
			qCritical()<<"Nemo has finished executing, but task is not defined";
	}
}

/*! Sets the parameters of the simulation */
void NemoWidget::setParameters(){

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
		nemoWrapper->preparePlaySimulation();
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

