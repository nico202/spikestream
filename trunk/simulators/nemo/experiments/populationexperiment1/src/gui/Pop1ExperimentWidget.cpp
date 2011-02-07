//SpikeStream includes
#include "Globals.h"
#include "ParametersDialog.h"
#include "Pop1ExperimentWidget.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>


//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new Pop1ExperimentWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Population Learning Experiment 1");
	}
}


/*! Constructor */
Pop1ExperimentWidget::Pop1ExperimentWidget(QWidget* parent) : AbstractExperimentWidget(parent){
	//Add toolbar to start and stop experiment
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QToolBar* toolBar = getToolBar();
	parametersButton = new QPushButton("Parameters");
	connect(parametersButton, SIGNAL(clicked()), this, SLOT(parametersButtonClicked()));
	parametersButton->setMaximumSize(100, 20);
	parametersButton->setMinimumSize(100, 20);
	toolBar->addWidget(parametersButton);
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
	pop1ExperimentManager = new Pop1ExperimentManager();
	connect(pop1ExperimentManager, SIGNAL(finished()), this, SLOT(managerFinished()));
	connect(pop1ExperimentManager, SIGNAL(statusUpdate(QString)), this, SLOT(updateStatus(QString)));
}


/*! Destructor */
Pop1ExperimentWidget::~Pop1ExperimentWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the wrapper that will be used in the experiments.
	The void pointer must point to an object of class NemoWrapper */
void Pop1ExperimentWidget::setWrapper(void *wrapper){
	this->nemoWrapper = (NemoWrapper*)wrapper;
}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

/*! Called when the manager exits */
void Pop1ExperimentWidget::managerFinished(){
	//Check for errors
	if(pop1ExperimentManager->isError()){
		qCritical()<<pop1ExperimentManager->getErrorMessage();
	}

	//Signal to other classes that experiment is complete
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	emit experimentEnded();
}


/*! Called when the parameters button is clicked. Sets the parameters. */
void Pop1ExperimentWidget::parametersButtonClicked(){
	try{
		ParametersDialog dialog(parameterInfoList, defaultParameterMap, parameterMap, this);
		if(dialog.exec() == QDialog::Accepted){
			parameterMap = dialog.getParameters();
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


//Inherited from AbstractExperimentWidget
void Pop1ExperimentWidget::startExperiment(){
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
		pop1ExperimentManager->startExperiment(nemoWrapper, neuronGroup, parameterMap);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


//Inherited from AbstractExperimentWidget
void Pop1ExperimentWidget::stopExperiment(){
	pop1ExperimentManager->stop();
}


/*! Adds the specified message to the status pane */
void Pop1ExperimentWidget::updateStatus(QString statusMsg){
	statusTextEdit->append(statusMsg);
}


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Builds the current and default parameter map along with the parameter
	info list. */
void Pop1ExperimentWidget::buildParameters(){
	ParameterInfo tmpInfo("experiment_number", "The experiment that is carried out", ParameterInfo::OPTION);
	QList<QString> exptNameList;
	exptNameList.append("Experiment 1 - Sequence Learning");
	exptNameList.append("Experiment 2 - Sequence Learning with Noise");
	tmpInfo.setOptionNames(exptNameList);
	parameterInfoList.append(tmpInfo);
	defaultParameterMap["experiment_number"] = 0;
	parameterMap["experiment_number"] = defaultParameterMap["experiment_number"];

	parameterInfoList.append(ParameterInfo("number_of_patterns", "Number of patterns to learn in a sequence", ParameterInfo::INTEGER));
	defaultParameterMap["number_of_patterns"] = 5;
	parameterMap["number_of_patterns"] = defaultParameterMap["number_of_patterns"];

	parameterInfoList.append(ParameterInfo("random_seed", "Seed for random number generator.", ParameterInfo::INTEGER));
	defaultParameterMap["random_seed"] = 50;
	parameterMap["random_seed"] = defaultParameterMap["random_seed"];

	parameterInfoList.append(ParameterInfo("number_of_x_neurons", "Number of neurons along the X axis", ParameterInfo::INTEGER));
	defaultParameterMap["number_of_x_neurons"] = 10;
	parameterMap["number_of_x_neurons"] = defaultParameterMap["number_of_x_neurons"];

	parameterInfoList.append(ParameterInfo("number_of_y_neurons", "Number of neurons along the Y axis", ParameterInfo::INTEGER));
	defaultParameterMap["number_of_y_neurons"] = 10;
	parameterMap["number_of_y_neurons"] = defaultParameterMap["number_of_y_neurons"];

	parameterInfoList.append(ParameterInfo("x_start", "Start position of layer on X axis", ParameterInfo::INTEGER));
	defaultParameterMap["x_start"] = 1;
	parameterMap["x_start"] = defaultParameterMap["x_start"];

	parameterInfoList.append(ParameterInfo("y_start", "Start position of layer on Y axis", ParameterInfo::INTEGER));
	defaultParameterMap["y_start"] = 1;
	parameterMap["y_start"] = defaultParameterMap["y_start"];

	parameterInfoList.append(ParameterInfo("z_start", "Start position of layer on Z axis", ParameterInfo::INTEGER));
	defaultParameterMap["z_start"] = 1;
	parameterMap["z_start"] = defaultParameterMap["z_start"];

	parameterInfoList.append(ParameterInfo("num_random_shift_cols", "Number of columns to shift by the random shift amount", ParameterInfo::INTEGER));
	defaultParameterMap["num_random_shift_cols"] = 1;
	parameterMap["num_random_shift_cols"] = defaultParameterMap["num_random_shift_cols"];

	parameterInfoList.append(ParameterInfo("random_shift_amount", "Amount to shift each Y value by in experiment 2", ParameterInfo::INTEGER));
	defaultParameterMap["random_shift_amount"] = 1;
	parameterMap["random_shift_amount"] = defaultParameterMap["random_shift_amount"];

	parameterInfoList.append(ParameterInfo("steps_to_pattern_read", "Number of steps between injecting pattern and reading back the resulting pattern", ParameterInfo::INTEGER));
	defaultParameterMap["steps_to_pattern_read"] = 12;
	parameterMap["steps_to_pattern_read"] = defaultParameterMap["steps_to_pattern_read"];

	parameterInfoList.append(ParameterInfo("pause_interval_ms", "Time in ms to pause between sections of the experiment", ParameterInfo::INTEGER));
	defaultParameterMap["pause_interval_ms"] = 1000;
	parameterMap["pause_interval_ms"] = defaultParameterMap["pause_interval_ms"];


}

/*! Checks that the network has the right number of neurons for the experiments. */
void Pop1ExperimentWidget::checkNetwork(){
	Network* tmpNetwork = Globals::getNetwork();
	QList<NeuronGroup*> neurGrpList = tmpNetwork->getNeuronGroups();
	if(neurGrpList.size() != 1)
		throw SpikeStreamException("Experiment is expecting 1 neuron group to be present in the network.");
	if(neurGrpList.at(0)->size() != 100)
		throw SpikeStreamException("Experiment is expecting 100 neurons to be present in the network.");
	neuronGroup = neurGrpList.at(0);
}



