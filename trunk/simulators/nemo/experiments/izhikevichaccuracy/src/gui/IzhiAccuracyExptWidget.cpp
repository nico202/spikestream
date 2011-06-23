//SpikeStream includes
#include "Globals.h"
#include "IzhiAccuracyExptWidget.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>


//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new IzhiAccuracyExptWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Accuracy of NeMo Simulation");
	}
}


/*! Constructor */
IzhiAccuracyExptWidget::IzhiAccuracyExptWidget(QWidget* parent) : AbstractExperimentWidget(parent){
	//Add toolbar to start and stop experiment
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QToolBar* toolBar = getToolBar();
	mainVBox->addWidget(toolBar);

	//Add status display area
	statusTextEdit = new QTextEdit(this);
	statusTextEdit->setReadOnly(true);
	mainVBox->addWidget(statusTextEdit);
	mainVBox->addStretch(5);

	//Initialize variables
	nemoWrapper = NULL;
	buildParameters();

	//Create experiment manager to run experiment
	izhiAccuracyManager = new IzhiAccuracyManager();
	connect(izhiAccuracyManager, SIGNAL(finished()), this, SLOT(managerFinished()));
	connect(izhiAccuracyManager, SIGNAL(statusUpdate(QString)), this, SLOT(updateStatus(QString)));
}


/*! Destructor */
IzhiAccuracyExptWidget::~IzhiAccuracyExptWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the wrapper that will be used in the experiments.
	The void pointer must point to an object of class NemoWrapper */
void IzhiAccuracyExptWidget::setWrapper(void *wrapper){
	this->nemoWrapper = (NemoWrapper*)wrapper;
}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

/*! Called when the manager exits */
void IzhiAccuracyExptWidget::managerFinished(){
	//Check for errors
	if(izhiAccuracyManager->isError()){
		qCritical()<<izhiAccuracyManager->getErrorMessage();
	}

	//Signal to other classes that experiment is complete
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	emit experimentEnded();
}


//Inherited from AbstractExperimentWidget
void IzhiAccuracyExptWidget::startExperiment(){
	//Run some checks before experiment can proceed.
	if(nemoWrapper == NULL){
		qCritical()<<"NemoWrapper has not been set.";
		return;
	}
	if(!nemoWrapper->isSimulationLoaded()){
		qCritical()<<"Experiment cannot be started until NemoWrapper has a loaded simulation.";
		return;
	}
	try{
		checkNetwork();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		return;
	}

	//Start experiment and signal that it has started.
	statusTextEdit->clear();
	playAction->setEnabled(false);
	stopAction->setEnabled(true);
	emit experimentStarted();
	try{
		izhiAccuracyManager->startExperiment(nemoWrapper, parameterMap);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		managerFinished();
	}
}


//Inherited from AbstractExperimentWidget
void IzhiAccuracyExptWidget::stopExperiment(){
	izhiAccuracyManager->stop();
}


/*! Adds the specified message to the status pane */
void IzhiAccuracyExptWidget::updateStatus(QString statusMsg){
	statusTextEdit->append(statusMsg);
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Builds the current and default parameter map along with the parameter
	info list. */
void IzhiAccuracyExptWidget::buildParameters(){
	//Reset maps and list
	parameterInfoList.clear();
	defaultParameterMap.clear();
	parameterMap.clear();

	ParameterInfo tmpInfo("experiment_number", "The experiment that is carried out", ParameterInfo::OPTION);
	QList<QString> exptNameList;
	exptNameList.append("Experiment 1 - Accuracy test");
	exptNameList.append("Experiment 2 - Monitoring test");
	tmpInfo.setOptionNames(exptNameList);
	parameterInfoList.append(tmpInfo);
	defaultParameterMap["experiment_number"] = 0;
	parameterMap["experiment_number"] = defaultParameterMap["experiment_number"];

	parameterInfoList.append(ParameterInfo("random_seed", "Seed for random number generator.", ParameterInfo::INTEGER));
	defaultParameterMap["random_seed"] = 45;
	parameterMap["random_seed"] = defaultParameterMap["random_seed"];

	parameterInfoList.append(ParameterInfo("pause_interval_ms", "Time in ms to pause between sections of the experiment", ParameterInfo::INTEGER));
	defaultParameterMap["pause_interval_ms"] = 1000;
	parameterMap["pause_interval_ms"] = defaultParameterMap["pause_interval_ms"];
}


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Checks that the network has the right number of neurons for the experiments. */
void IzhiAccuracyExptWidget::checkNetwork(){
	Network* tmpNetwork = Globals::getNetwork();
	QList<NeuronGroup*> neurGrpList = tmpNetwork->getNeuronGroups();

	//Check that input layer, temporal coding and feature detection are there
	bool excitLayerFound = false, inhibLayerFound = false;
	foreach(NeuronGroup* tmpNeurGrp, neurGrpList){
		if(tmpNeurGrp->getInfo().getName().toUpper() == "EXCITATORY NEURON GROUP"){
			if(tmpNeurGrp->size() != 800)
				throw SpikeStreamException("Excitatory neuron group should contain 800 neurons");
			excitLayerFound = true;
		}
		if(tmpNeurGrp->getInfo().getName().toUpper() == "INHIBITORY NEURON GROUP"){
			if(tmpNeurGrp->size() != 200)
				throw SpikeStreamException("Inhibitory neuron group should contain 200 neurons");
			inhibLayerFound = true;
		}
		if(excitLayerFound && inhibLayerFound)
			break;
	}

	//Throw exception if we have not found the necessary neuron groups
	if(!excitLayerFound || ! inhibLayerFound)
		throw SpikeStreamException("Excitatory or inhibitory layers could not be found.\nMake sure there are neuron groups labelled 'Excitatory neuron group' and 'Inhibitory neuron group' in your network.");
}



