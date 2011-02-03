//SpikeStream includes
#include "Globals.h"
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
	mainVBox->addWidget(getToolBar());

	//Initialize variables
	nemoWrapper = NULL;

	//Create experiment manager to run experiment
	pop1ExperimentManager = new Pop1ExperimentManager();
	connect(pop1ExperimentManager, SIGNAL(finished()), this, SLOT(managerFinished()));
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
	emit experimentEnded();
}


//Inherited from AbstractExperimentWidget
void Pop1ExperimentWidget::startExperiment(){
	//Run some checks before experiment can proceed.
	if(nemoWrapper == NULL)
		throw SpikeStreamException("NemoWrapper has not been set.");
	if(!nemoWrapper->isSimulationLoaded())
		throw SpikeStreamException("Experiment cannot be started until NemoWrapper has a loaded simulation.");
	checkNetwork();

	//Start experiment and signal that it has started.
	emit experimentStarted();
	try{
		pop1ExperimentManager->startExperiment();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


//Inherited from AbstractExperimentWidget
void Pop1ExperimentWidget::stopExperiment(){
	pop1ExperimentManager->stop();
}


/*----------------------------------------------------------*/
/*------                PRIVATE METHODS               ------*/
/*----------------------------------------------------------*/

/*! Checks that the network has the right number of neurons for the experiments. */
void Pop1ExperimentWidget::checkNetwork(){

}



