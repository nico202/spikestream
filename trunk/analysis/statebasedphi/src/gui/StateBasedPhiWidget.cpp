//SpikeStream includes
#include "ExportAnalysisDialog.h"
#include "FullResultsTableView.h"
#include "Globals.h"
#include "LoadAnalysisDialog.h"
#include "SpikeStreamException.h"
#include "StateBasedPhiWidget.h"
#include "StateBasedPhiParameterDialog.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QDebug>
#include <QIcon>
#include <QMessageBox>
#include <QSqlQueryModel>
#include <QScrollArea>
#include <QTabWidget>

//Other includes
#include <iostream>
using namespace std;


//Functions for dynamic library loading
extern "C" {
    /*! Creates a StateBasedPhiWidget class when library is dynamically loaded. */
    StateBasedPhiWidget* getClass(){
	return new StateBasedPhiWidget();
    }

    /*! Returns a sensible name for this widget */
    QString getName(){
	return QString("State-based Phi Analysis");
    }
}


/*! Constructor */
StateBasedPhiWidget::StateBasedPhiWidget(QWidget *parent) : QWidget(parent){
    QVBoxLayout *mainVerticalBox = new QVBoxLayout(this);

    //Create a state based phi analysis dao to be used by this class
    stateDao = new StateBasedPhiAnalysisDao(Globals::getAnalysisDao()->getDBInfo());

    //Initialize analysis parameters and other variables
    initializeAnalysisInfo();
    currentTask = UNDEFINED_TASK;

    //Add tool bar to top of widget
    toolBar = getToolBar();
    checkToolBarEnabled();//Can only carry out analysis if a network and archive are loaded
    mainVerticalBox->addWidget(toolBar);

    //Create a tabbed widget to hold progress and results
    QTabWidget* tabWidget = new QTabWidget(this);

    //Add the model and view displaying the current analysis
    fullResultsModel = new FullResultsModel(&analysisInfo, stateDao);
    QTableView* fullResultsTableView = new FullResultsTableView(fullResultsModel);
    fullResultsTableView->setMinimumSize(500, 500);
    tabWidget->addTab(fullResultsTableView, "Results");

    //Add widget displaying progress
    progressWidget = new ProgressWidget(this);
    progressWidget->setMinimumSize(500, 500);
    QScrollArea* progressScrollArea = new QScrollArea(this);
    progressScrollArea->setWidget(progressWidget);
    tabWidget->addTab(progressScrollArea, "Progress");
    mainVerticalBox->addWidget(tabWidget);

    //Listen for events that affect whether the tool bar should be enabled or not.
    connect(Globals::getEventRouter(), SIGNAL(archiveChangedSignal()), this, SLOT(archiveChanged()));
    connect(Globals::getEventRouter(), SIGNAL(archiveChangedSignal()), this, SLOT(loadArchiveTimeStepsIntoCombos()));
    connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));

    //Set up class to run analysis
    analysisRunner = new AnalysisRunner(
	    Globals::getNetworkDao()->getDBInfo(),
	    Globals::getArchiveDao()->getDBInfo(),
	    Globals::getAnalysisDao()->getDBInfo()
    );
    connect(analysisRunner, SIGNAL(finished()), this, SLOT(threadFinished()));
    connect(analysisRunner, SIGNAL(finished()), Globals::getEventRouter(), SLOT(analysisStopped()));
    connect(analysisRunner, SIGNAL(progress(const QString&, unsigned int, unsigned int, unsigned int)), progressWidget, SLOT(updateProgress(const QString&, unsigned int, unsigned int, unsigned int)), Qt::QueuedConnection);
    connect(analysisRunner, SIGNAL(timeStepComplete(unsigned int)), progressWidget, SLOT(timeStepComplete(unsigned int)), Qt::QueuedConnection);
    connect(analysisRunner, SIGNAL(complexFound()), this , SLOT(updateResults()), Qt::QueuedConnection);
}


/*! Destructor */
StateBasedPhiWidget::~StateBasedPhiWidget(){
    delete analysisRunner;
    delete stateDao;
    delete fullResultsModel;
}


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

/*! Called when the archive changes and sets archive ID appropriately */
void StateBasedPhiWidget::archiveChanged(){
    //The analyses are archive specific, so create a new analysis
    newAnalysis();
}


/*! Exports data from the analysis */
void StateBasedPhiWidget::exportAnalysis(){
    //Show dialog to select the analysis the user wants to load
    ExportAnalysisDialog exportAnalysisDialog(this);
    exportAnalysisDialog.exec();
}


/*! Called when the network changes and sets network ID appropriately */
void StateBasedPhiWidget::networkChanged(){
    //The analyses are archive specific, so create a new analysis
    newAnalysis();
}


/*! Checks to see if network or archive have been loaded */
void StateBasedPhiWidget::checkToolBarEnabled(){
    if(Globals::networkLoaded() && Globals::archiveLoaded())
	toolBar->setEnabled(true);
    else
	toolBar->setEnabled(false);
}


/*! Corrects the time step selection combos so that the from is never greater than the to */
void StateBasedPhiWidget::fixTimeStepSelection(int selectedIndex){
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


/*! Loads up the minimum and maximum time step for the archive */
void StateBasedPhiWidget::loadArchiveTimeStepsIntoCombos(){
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


/*! Displays the load analysis dialog box so the user can select which analysis to load */
void StateBasedPhiWidget::loadAnalysis(){
    try{
	//Show dialog to select the analysis the user wants to load
	LoadAnalysisDialog loadAnalysisDialog(this);
	if(loadAnalysisDialog.exec() == QDialog::Accepted ) {//Load the archive
	    analysisInfo = loadAnalysisDialog.getAnalysisInfo();
	    loadAnalysisResults();
	    Globals::setAnalysisID(analysisInfo.getID());
	}
    }
    catch (SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
	Globals::setAnalysisID(0);//Clear analysis status in globals
	return;
    }
}


/*! Resets everything ready for a new analysis */
void StateBasedPhiWidget::newAnalysis(){
    //Set enabled status of toolbar
    checkToolBarEnabled();

    //Reset the analysis info
    initializeAnalysisInfo();

    //Clear analysis id
    Globals::setAnalysisID(0);

    //Reload model
    fullResultsModel->reload();

    //Reset progress widget
    progressWidget->reset();
}


/*! Selects the parameters to be used for the analysis, including the description.
    These cannot be edited once the analysis has been started - otherwise would have to associate
    a set of parameter with each time step */
void StateBasedPhiWidget::selectParameters(){
    //Record the current description
    QString oldDescription = analysisInfo.getDescription();

    StateBasedPhiParameterDialog dialog(this, analysisInfo);
    if(dialog.exec() == QDialog::Accepted ) {
	//Copy the new information that has been set
	analysisInfo = dialog.getInfo();

	//Update the description if it has changed
	if(analysisInfo.getDescription() != oldDescription && analysisInfo.getDescription() != "")
	    stateDao->updateDescription(analysisInfo.getID(), analysisInfo.getDescription());
    }
}


/*! Starts the analysis of the network for state-based phi */
void StateBasedPhiWidget::startAnalysis(){
    //Double check that both a network and an analysis are loaded
    if(!Globals::networkLoaded() || !Globals::archiveLoaded()){
	qCritical()<<"Network and/or archive not loaded - cannot start analysis.";
	return;
    }

    //Get time steps to be analyzed
    int firstTimeStep = Util::getInt(fromTimeStepCombo->currentText());
    int lastTimeStep = Util::getInt(toTimeStepCombo->currentText());

    //Create a new analysis in the database if one is not loaded
    try {
	if(!Globals::analysisLoaded()){
	    Globals::getAnalysisDao()->addAnalysis(analysisInfo);

	    //Analysis id in analysisInfo should now be set
	    if(analysisInfo.getID() == 0){
		qCritical()<<"Analysis has not been added correctly";
		return;
	    }

	    //Store analysis ID in Globals to indicate that it is now loaded
	    Globals::setAnalysisID(analysisInfo.getID());
	}

	//Check for a time step conflict
	if(timeStepsAlreadyAnalyzed(firstTimeStep, lastTimeStep)){
	    //Check to see if user wants to overwrite time steps fully or partly analyzed
	    QString confirmMsg = "Some or all of the time steps from " + QString::number(firstTimeStep);
	    confirmMsg += " to " + QString::number(lastTimeStep) + " have already been fully or partly analyzed.\n";
	    confirmMsg += "Do you want to overwrite the current results for these time steps in the database?";
	    QMessageBox::StandardButton response = QMessageBox::warning(this, "Time step conflict", confirmMsg, QMessageBox::Ok | QMessageBox::Cancel);
	    if(response != QMessageBox::Ok)
		return;

	    //Delete time steps from the database and refresh the list of complexes
	    stateDao->deleteTimeSteps(firstTimeStep, lastTimeStep);
	    loadAnalysisResults();
	}

	//Initialize classes to run analysis
	progressWidget->reset();
	analysisRunner->prepareAnalysisTask(analysisInfo, firstTimeStep, lastTimeStep);
	currentTask = ANALYSIS_TASK;
	analysisRunner->start();
	Globals::getEventRouter()->analysisStarted();
    }
    catch(SpikeStreamException& ex){
	qCritical()<<ex.getMessage();

	//We don't know if analysis runner has started or not, so set error just in case
	analysisRunner->setError("Exception thrown starting task or adding analysis to database.");
    }
}


/*! Stops the analysis of the network. */
void StateBasedPhiWidget::stopAnalysis(){
    analysisRunner->stop();
}


/*! Called when the thread running the analysis finishes. */
void StateBasedPhiWidget::threadFinished(){
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


/*! Updates results table by reloading it from the database */
void StateBasedPhiWidget::updateResults(){
    loadAnalysisResults();
}


/*-------------------------------------------------------------*/
/*-------                PRIVATE METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Builds a list of time steps covering the specified range. */
QStringList StateBasedPhiWidget::getTimeStepList(unsigned int min, unsigned int max){
    QStringList tmpStrList;
    for(unsigned int i=min; i<=max; ++i)
	tmpStrList.append(QString::number(i));
    return tmpStrList;
}


/*! Builds the toolbar that goes at the top of the page. */
QToolBar* StateBasedPhiWidget::getToolBar(){
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

    return tmpToolBar;
}


/*! Sets initial state of parameters for a state based phi analysis */
void StateBasedPhiWidget::initializeAnalysisInfo(){
    //Initialize all values in the analysis info to default values
    analysisInfo.reset();

    //Set the network id
    if(Globals::networkLoaded())
	analysisInfo.setNetworkID(Globals::getNetwork()->getID());
    else
	analysisInfo.setNetworkID(0);

    //Set archive id
    if(Globals::archiveLoaded())
	analysisInfo.setArchiveID(Globals::getArchive()->getID());
    else
	analysisInfo.setArchiveID(0);

    //Set the type of analysis, 1 corresponds to a state based phi analysis
    analysisInfo.setAnalysisType(1);

    //Set parameters
    analysisInfo.getParameterMap()["Generalization"] = 1.0;
}


/*! Instructs the model to reload the results from the database */
void StateBasedPhiWidget::loadAnalysisResults(){
    fullResultsModel->reload();
}


/*! Checks to see if any of the specified range of time steps already exist in the database */
bool StateBasedPhiWidget::timeStepsAlreadyAnalyzed(int firstTimeStep, int lastTimeStep){
    if(!Globals::analysisLoaded()){
	return false;
    }

    int complexCount = stateDao->getComplexCount(analysisInfo.getID(), firstTimeStep, lastTimeStep);
    if(complexCount > 0)
	return true;
    return false;
}


