//SpikeStream includes
#include "Globals.h"
#include "MasquelierCodingeEptWidget.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>


//Functions for dynamic library loading
extern "C" {
        /*! Creates an QWidget class when library is dynamically loaded. */
        QWidget* getClass(){
                return new MasquelierCodingeEptWidget();
        }

        /*! Returns a descriptive name for this widget */
        QString getName(){
                return QString("Masquelier Learning Experiment 1");
        }
}


/*! Constructor */
MasquelierCodingeEptWidget::MasquelierCodingeEptWidget(QWidget* parent) : AbstractExperimentWidget(parent){
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
        masquelierCodingeEptManager = new MasquelierCodingeEptManager();
        connect(masquelierCodingeEptManager, SIGNAL(finished()), this, SLOT(managerFinished()));
        connect(masquelierCodingeEptManager, SIGNAL(statusUpdate(QString)), this, SLOT(updateStatus(QString)));
}


/*! Destructor */
MasquelierCodingeEptWidget::~MasquelierCodingeEptWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Sets the wrapper that will be used in the experiments.
        The void pointer must point to an object of class NemoWrapper */
void MasquelierCodingeEptWidget::setWrapper(void *wrapper){
        this->nemoWrapper = (NemoWrapper*)wrapper;
}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

/*! Called when the manager exits */
void MasquelierCodingeEptWidget::managerFinished(){
        //Check for errors
        if(masquelierCodingeEptManager->isError()){
                qCritical()<<masquelierCodingeEptManager->getErrorMessage();
        }

        //Signal to other classes that experiment is complete
        playAction->setEnabled(true);
        stopAction->setEnabled(false);
        emit experimentEnded();
}


//Inherited from AbstractExperimentWidget
void MasquelierCodingeEptWidget::startExperiment(){
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
                masquelierCodingeEptManager->startExperiment(nemoWrapper, parameterMap);
        }
        catch(SpikeStreamException& ex){
                qCritical()<<ex.getMessage();
                managerFinished();
        }
}


//Inherited from AbstractExperimentWidget
void MasquelierCodingeEptWidget::stopExperiment(){
        masquelierCodingeEptManager->stop();
}


/*! Adds the specified message to the status pane */
void MasquelierCodingeEptWidget::updateStatus(QString statusMsg){
        statusTextEdit->append(statusMsg);
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Builds the current and default parameter map along with the parameter
        info list. */
void MasquelierCodingeEptWidget::buildParameters(){
        //Reset maps and list
        parameterInfoList.clear();
        defaultParameterMap.clear();
        parameterMap.clear();

        ParameterInfo tmpInfo("experiment_number", "The experiment that is carried out", ParameterInfo::OPTION);
        QList<QString> exptNameList;
        exptNameList.append("Experiment 1 - Generate artificial spiketrain");
        tmpInfo.setOptionNames(exptNameList);
        parameterInfoList.append(tmpInfo);
        defaultParameterMap["experiment_number"] = 0;
        parameterMap["experiment_number"] = defaultParameterMap["experiment_number"];

        parameterInfoList.append(ParameterInfo("no_afferents", "Number of afferents.", ParameterInfo::INTEGER));
        defaultParameterMap["no_afferents"] = 100;
        parameterMap["no_afferents"] = defaultParameterMap["no_afferents"];

        parameterInfoList.append(ParameterInfo("no_output_neurons_(MAXIMUM 9)", "Number of output neurons. Maximum of 9.", ParameterInfo::INTEGER));
        defaultParameterMap["no_output_neurons_(MAXIMUM 9)"] = 3;
        parameterMap["no_output_neurons_(MAXIMUM 9)"] = defaultParameterMap["no_output_neurons_(MAXIMUM 9)"];

        parameterInfoList.append(ParameterInfo("no_patterns", "Number of patterns to inject into each afferent.", ParameterInfo::INTEGER));
        defaultParameterMap["no_patterns"] = 1;
        parameterMap["no_patterns"] = defaultParameterMap["no_patterns"];

        parameterInfoList.append(ParameterInfo("percentage_afferents_including_pattern", "Number of afferents which include a pattern.", ParameterInfo::INTEGER));
        defaultParameterMap["percentage_afferents_including_pattern"] = 0.5;
        parameterMap["percentage_afferents_including_pattern"] = defaultParameterMap["percentage_afferents_including_pattern"];

        parameterInfoList.append(ParameterInfo("spontaneous_activity_hz", "Firing rate of spontaneous activity (Hz).", ParameterInfo::INTEGER));
        defaultParameterMap["spontaneous_activity_hz"] = 0;//0.5;
        parameterMap["spontaneous_activity_hz"] = defaultParameterMap["spontaneous_activity_hz"];

        parameterInfoList.append(ParameterInfo("pattern_duration_ms", "Duration of pattern (seconds).", ParameterInfo::INTEGER));
        defaultParameterMap["pattern_duration_ms"] = 0.05;
        parameterMap["pattern_duration_ms"] = defaultParameterMap["pattern_duration_ms"];

        parameterInfoList.append(ParameterInfo("max_firing_rate", "Maximum firing rate (hz).", ParameterInfo::INTEGER));
        defaultParameterMap["max_firing_rate"] = 10;
        parameterMap["max_firing_rate"] = defaultParameterMap["max_firing_rate"];

        parameterInfoList.append(ParameterInfo("time_seconds", "Time of experiment.", ParameterInfo::INTEGER));
        defaultParameterMap["time_seconds"] = 1600;
        parameterMap["time_seconds"] = defaultParameterMap["time_seconds"];

        parameterInfoList.append(ParameterInfo("jitter", "Jitter of each spike within pattern (seconds).", ParameterInfo::INTEGER));
        defaultParameterMap["jitter"] = 0;//0.001;
        parameterMap["jitter"] = defaultParameterMap["jitter"];

        parameterInfoList.append(ParameterInfo("pattern_frequency", "Proportion of time as patterns (percentage).", ParameterInfo::INTEGER));
        defaultParameterMap["pattern_frequency"] = 0.3333333;
        parameterMap["pattern_frequency"] = defaultParameterMap["pattern_frequency"];

        parameterInfoList.append(ParameterInfo("random_seed", "Seed for random number generator.", ParameterInfo::INTEGER));
        defaultParameterMap["random_seed"] = 1;
        parameterMap["random_seed"] = defaultParameterMap["random_seed"];

        parameterInfoList.append(ParameterInfo("pause_interval_ms", "Time in ms to pause between each step in the experiment", ParameterInfo::INTEGER));
        defaultParameterMap["pause_interval_ms"] = 0;
        parameterMap["pause_interval_ms"] = defaultParameterMap["pause_interval_ms"];
}


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Checks that the network has the right number of neurons for the experiments. */
void MasquelierCodingeEptWidget::checkNetwork(){
        Network* tmpNetwork = Globals::getNetwork();
        QList<NeuronGroup*> neurGrpList = tmpNetwork->getNeuronGroups();

        //Check that there is an input layer, and number of neurons equals the number of afferents
        //Also, check that the number of output neurons match those stated
        bool inputLayerFound = false;
        bool number_neurons = false;
        int output_neuron_count =0;
        foreach(NeuronGroup* tmpNeurGrp, neurGrpList){
                if(tmpNeurGrp->getInfo().getName().toUpper() == "INPUT LAYER"){
                        inputLayerFound = true;
                        if(parameterMap["experiment_number"] == 0 && tmpNeurGrp->size() == parameterMap["no_afferents"])
                            number_neurons = true;
                }
                if(tmpNeurGrp->getInfo().getName().toUpper() == "OUTPUT NEURON"){
                        if(parameterMap["experiment_number"] == 0 && tmpNeurGrp->size() == 1)
                            output_neuron_count++;
                }
        }

        //Throw exception if we have not found the input group with correct number of neurons
        if(!inputLayerFound)
                throw SpikeStreamException("Input layer could not be found.\nMake sure there is a neuron group labelled 'Input layer'");
        if(!number_neurons)
                throw SpikeStreamException("Incorrect number of neurons. \n Ensure #neurons = #afferents in 'Input layer'");
        if(output_neuron_count !=  parameterMap["no_output_neurons_(MAXIMUM 9)"])
                throw SpikeStreamException("Incorrect number of output neurons. Possible solutions: \n i)Ensure # output neurons = #output neurons stated \n ii)Ensure that each output neuron is maximum size of 1 \n iii)Ensure each output neuron is labelled as OUTPUT NEURON");
        if(output_neuron_count > 9)
                throw SpikeStreamException("Too many output neurons, select a maximum of 9");
}

