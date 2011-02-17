//SpikeStream includes
#include "Globals.h"
#include "TemporalCodingExptWidget.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>


//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new TemporalCodingExptWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Temporal Coding Learning Experiment 1");
	}
}


/*! Constructor */
TemporalCodingExptWidget::TemporalCodingExptWidget(QWidget* parent) : AbstractExperimentWidget(parent){
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
	temporalCodingExptManager = new TemporalCodingExptManager();
	connect(temporalCodingExptManager, SIGNAL(finished()), this, SLOT(managerFinished()));
	connect(temporalCodingExptManager, SIGNAL(statusUpdate(QString)), this, SLOT(updateStatus(QString)));
}


/*! Destructor */
TemporalCodingExptWidget::~TemporalCodingExptWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the wrapper that will be used in the experiments.
	The void pointer must point to an object of class NemoWrapper */
void TemporalCodingExptWidget::setWrapper(void *wrapper){
	this->nemoWrapper = (NemoWrapper*)wrapper;
}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

/*! Called when the manager exits */
void TemporalCodingExptWidget::managerFinished(){
	//Check for errors
	if(temporalCodingExptManager->isError()){
		qCritical()<<temporalCodingExptManager->getErrorMessage();
	}

	//Signal to other classes that experiment is complete
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	emit experimentEnded();
}


//Inherited from AbstractExperimentWidget
void TemporalCodingExptWidget::startExperiment(){
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
		temporalCodingExptManager->startExperiment(nemoWrapper, parameterMap);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		managerFinished();
	}
}


//Inherited from AbstractExperimentWidget
void TemporalCodingExptWidget::stopExperiment(){
	temporalCodingExptManager->stop();
}


/*! Adds the specified message to the status pane */
void TemporalCodingExptWidget::updateStatus(QString statusMsg){
	statusTextEdit->append(statusMsg);
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Builds the current and default parameter map along with the parameter
	info list. */
void TemporalCodingExptWidget::buildParameters(){
	//Reset maps and list
	parameterInfoList.clear();
	defaultParameterMap.clear();
	parameterMap.clear();

	ParameterInfo tmpInfo("experiment_number", "The experiment that is carried out", ParameterInfo::OPTION);
	QList<QString> exptNameList;
	exptNameList.append("Experiment 1 - Up trend");
	exptNameList.append("Experiment 2 - Down trend");
	exptNameList.append("Experiment 3 - Down/up trend");
	exptNameList.append("Experiment 4 - Up/down trend");
	tmpInfo.setOptionNames(exptNameList);
	parameterInfoList.append(tmpInfo);
	defaultParameterMap["experiment_number"] = 0;
	parameterMap["experiment_number"] = defaultParameterMap["experiment_number"];

	parameterInfoList.append(ParameterInfo("random_seed", "Seed for random number generator.", ParameterInfo::INTEGER));
	defaultParameterMap["random_seed"] = 50;
	parameterMap["random_seed"] = defaultParameterMap["random_seed"];

	parameterInfoList.append(ParameterInfo("pause_interval_ms", "Time in ms to pause between each step in the experiment", ParameterInfo::INTEGER));
	defaultParameterMap["pause_interval_ms"] = 1000;
	parameterMap["pause_interval_ms"] = defaultParameterMap["pause_interval_ms"];

	parameterInfoList.append(ParameterInfo("num_time_steps", "Number of time steps that pattern extends over", ParameterInfo::INTEGER));
	defaultParameterMap["num_time_steps"] = 10;
	parameterMap["num_time_steps"] = defaultParameterMap["num_time_steps"];

	parameterInfoList.append(ParameterInfo("num_inter_expt_steps", "Number of time steps to run simulator in between each experiment. Should be enough to clear activity and refractory periods in the network.", ParameterInfo::INTEGER));
	defaultParameterMap["num_inter_expt_steps"] = 50;
	parameterMap["num_inter_expt_steps"] = defaultParameterMap["num_inter_expt_steps"];

	parameterInfoList.append(ParameterInfo("num_result_steps", "Number of time steps to run simulator after a pattern has been injected before reading result.", ParameterInfo::INTEGER));
	defaultParameterMap["num_result_steps"] = 20;
	parameterMap["num_result_steps"] = defaultParameterMap["num_result_steps"];
}


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Checks that the network has the right number of neurons for the experiments. */
void TemporalCodingExptWidget::checkNetwork(){
	Network* tmpNetwork = Globals::getNetwork();
	QList<NeuronGroup*> neurGrpList = tmpNetwork->getNeuronGroups();

	//Check that input layer, temporal coding and feature detection are there
	bool inputLayerFound = false, featureLayerFound = false;
	foreach(NeuronGroup* tmpNeurGrp, neurGrpList){
		if(tmpNeurGrp->getInfo().getName().toUpper() == "INPUT LAYER"){
			inputLayerFound = true;
		}
		if(tmpNeurGrp->getInfo().getName().toUpper() == "FEATURE DETECTION"){
			featureLayerFound = true;
		}

		//Exit loop if all layers found
		if(inputLayerFound && featureLayerFound)
			break;
	}

	//Throw exception if we have not found the necessary neuron groups
	if(!inputLayerFound || ! featureLayerFound)
		throw SpikeStreamException("Input and/or feature layers could not be found.\nMake sure there are neuron groups labelled 'Input layer' and 'Feature detection' in your network.");
}



