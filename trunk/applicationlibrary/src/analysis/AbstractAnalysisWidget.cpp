//SpikeStream includes
#include "AbstractAnalysisWidget.h"
#include "AnalysisParameterDialog.h"
#include "Globals.h"
#include "LoadAnalysisDialog.h"
#include "SpikeStreamAnalysisException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QAction>


/*! Constructor */
AbstractAnalysisWidget::AbstractAnalysisWidget(QWidget* parent) : QWidget(parent){
	currentTask = UNDEFINED_TASK;

	//Set up class to run analysis
	analysisRunner = new AnalysisRunner(
			Globals::getNetworkDao()->getDBInfo(),
			Globals::getArchiveDao()->getDBInfo(),
			Globals::getAnalysisDao()->getDBInfo()
			);
	connect(analysisRunner, SIGNAL(finished()), this, SLOT(threadFinished()));
	connect(analysisRunner, SIGNAL(finished()), Globals::getEventRouter(), SLOT(analysisStopped()));
	connect(analysisRunner, SIGNAL(newResultsFound()), this , SLOT(updateResults()), Qt::QueuedConnection);


	//Listen for events that affect whether the tool bar should be enabled or not.
	connect(Globals::getEventRouter(), SIGNAL(archiveChangedSignal()), this, SLOT(archiveChanged()));
	connect(Globals::getEventRouter(), SIGNAL(archiveChangedSignal()), this, SLOT(loadArchiveTimeStepsIntoCombos()));
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));
}


/*! Destructor */
AbstractAnalysisWidget::~AbstractAnalysisWidget(){
	delete analysisRunner;
}


/*----------------------------------------------------------*/
/*------              PROTECTED SLOTS                 ------*/
/*----------------------------------------------------------*/

/*! Called when the archive changes and sets archive ID appropriately */
void AbstractAnalysisWidget::archiveChanged(){
	if(analysisRunner->isRunning())
		qCritical()<<"Archive should not be changed while analysis is running";

	//The analyses are archive specific, so create a new analysis
	newAnalysis();
}


/*! Corrects the time step selection combos so that the from is never greater than the to */
void AbstractAnalysisWidget::fixTimeStepSelection(int selectedIndex){
	//Check that from time step has not been set to greater than the to
	if(sender() == fromTimeStepCombo){//FROM time step combo generated the signal
		if(selectedIndex > toTimeStepCombo->currentIndex()){
			toTimeStepCombo->setCurrentIndex(selectedIndex);
		}
	}
	//Check to time step is not less than from
	else{//TO time step combo generated the signal
		if(selectedIndex < fromTimeStepCombo->currentIndex()){
			fromTimeStepCombo->setCurrentIndex(selectedIndex);
		}
	}
}


/*! Displays the load analysis dialog box so the user can select which analysis to load */
void AbstractAnalysisWidget::loadAnalysis(){
	try{
		//Show dialog to select the analysis the user wants to load
		LoadAnalysisDialog loadAnalysisDialog(this, analysisInfo.getAnalyisType());
		if(loadAnalysisDialog.exec() == QDialog::Accepted ) {//Load the archive
			analysisInfo = loadAnalysisDialog.getAnalysisInfo();
			updateResults();
			Globals::setAnalysisID(getAnalysisName(), analysisInfo.getID());
		}
	}
	catch (SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		Globals::setAnalysisID(getAnalysisName(), 0);
		return;
	}
}


/*! Loads up the minimum and maximum time step for the archive */
void AbstractAnalysisWidget::loadArchiveTimeStepsIntoCombos(){
	if(!Globals::archiveLoaded())
		return;

	unsigned int minTimeStep = Globals::getArchiveDao()->getMinTimeStep(Globals::getArchive()->getID());
	unsigned int maxTimeStep = Globals::getArchiveDao()->getMaxTimeStep(Globals::getArchive()->getID());
	QStringList timeStepList = getTimeStepList(minTimeStep, maxTimeStep);
	fromTimeStepCombo->clear();
	fromTimeStepCombo->addItems(timeStepList);
	toTimeStepCombo->clear();
	toTimeStepCombo->addItems(timeStepList);
	if(fromTimeStepCombo->count() > 0)
		fromTimeStepCombo->setCurrentIndex(0);
}


/*! Called when the network changes and sets network ID appropriately */
void AbstractAnalysisWidget::networkChanged(){
	if(analysisRunner->isRunning())
		qCritical()<<"Network should not be changed while analysis is running";

	//The analyses are archive specific, so create a new analysis
	newAnalysis();
}


/*! Selects the parameters to be used for the analysis, including the description.
	These cannot be edited once the analysis has been started - otherwise would have to associate
	a set of parameters with each time step */
void AbstractAnalysisWidget::selectParameters(){
	//Record the current description
	QString oldDescription = analysisInfo.getDescription();

	AnalysisParameterDialog dialog(this, analysisInfo);
	if(dialog.exec() == QDialog::Accepted ) {
		//Copy the new information that has been set
		analysisInfo = dialog.getInfo();

		//Update the description if it has changed
		if(analysisInfo.getDescription() != oldDescription && analysisInfo.getDescription() != "")
			Globals::getAnalysisDao()->updateDescription(analysisInfo.getID(), analysisInfo.getDescription());
	}
}


/*! Stops the analysis of the network. */
void AbstractAnalysisWidget::stopAnalysis(){
	analysisRunner->stop();
}


/*! Called when the thread running the analysis finishes. */
void AbstractAnalysisWidget::threadFinished(){
	//Check for errors
	if(analysisRunner->isError()){
		qCritical()<<analysisRunner->getErrorMessage();
		currentTask = UNDEFINED_TASK;
		return;
	}

	switch(currentTask){
		case ANALYSIS_TASK:
			Globals::getEventRouter()->analysisStopped();
				break;
		default:
				qCritical()<<"Current task not recognized: "<<currentTask;
	}
	currentTask = UNDEFINED_TASK;
}


/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Checks to see if network or archive have been loaded */
void AbstractAnalysisWidget::checkToolBarEnabled(){
	if(Globals::networkLoaded() && Globals::archiveLoaded())
		toolBar->setEnabled(true);
	else
		toolBar->setEnabled(false);
}


/*! Builds a list of time steps covering the specified range. */
QStringList AbstractAnalysisWidget::getTimeStepList(unsigned int min, unsigned int max){
	QStringList tmpStrList;
	for(unsigned int i=min; i<=max; ++i)
		tmpStrList.append(QString::number(i));
	return tmpStrList;
}


/*! Builds the toolbar that goes at the top of the page. */
QToolBar* AbstractAnalysisWidget::getDefaultToolBar(){
	QToolBar* tmpToolBar = new QToolBar(this);

	QAction* tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/open.png"), "Open", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(loadAnalysis()));
	tmpToolBar->addAction (tmpAction);

	tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/new.png"), "New", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(newAnalysis()));
	tmpToolBar->addAction (tmpAction);

	tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/save.png"), "Export analysis", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(exportAnalysis()));
	connect(Globals::getEventRouter(), SIGNAL(analysisNotRunningSignal(bool)), tmpAction, SLOT(setEnabled(bool)));
	tmpToolBar->addAction (tmpAction);

	tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/play.png"), "Start analysis", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(startAnalysis()));
	connect(Globals::getEventRouter(), SIGNAL(analysisNotRunningSignal(bool)), tmpAction, SLOT(setEnabled(bool)));
	tmpToolBar->addAction (tmpAction);

	tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/stop.png"), "Stop analysis", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(stopAnalysis()));
	tmpToolBar->addAction (tmpAction);

	fromTimeStepCombo = new QComboBox(this);
	tmpToolBar->addWidget(fromTimeStepCombo);
	connect(fromTimeStepCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fixTimeStepSelection(int)));

	toTimeStepCombo = new QComboBox(this);
	tmpToolBar->addWidget(toTimeStepCombo);
	connect(toTimeStepCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(fixTimeStepSelection(int)));

	tmpAction = new QAction(QIcon(), "Parameters", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(selectParameters()));
	tmpToolBar->addAction (tmpAction);

	tmpAction = new QAction(QIcon(), "Plot", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(plotGraphs()));
	tmpToolBar->addAction (tmpAction);

	return tmpToolBar;
}


/*! Returns the first time step to be analyzed  */
int AbstractAnalysisWidget::getFirstTimeStep(){
	return Util::getInt(fromTimeStepCombo->currentText());
}


/*! Returns the last time step to be analyzed */
int AbstractAnalysisWidget::getLastTimeStep(){
	return Util::getInt(toTimeStepCombo->currentText());
}

