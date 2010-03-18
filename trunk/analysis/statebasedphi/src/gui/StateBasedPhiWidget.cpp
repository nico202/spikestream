//SpikeStream includes
#include "StateBasedPhiExportDialog.h"
#include "FullResultsTableView.h"
#include "Globals.h"
#include "LoadAnalysisDialog.h"
#include "PhiAnalysisTimeStepThread.h"
#include "SpikeStreamException.h"
#include "StateBasedPhiWidget.h"
#include "AnalysisParameterDialog.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QDebug>
#include <QIcon>
#include <QMessageBox>
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


/*! Returns a class that is used by the analysis runner for the analysis */
AbstractAnalysisTimeStepThread* createAnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo){
	return new PhiAnalysisTimeStepThread(netDBInfo, archDBInfo, anaDBInfo);
}


/*! Constructor */
StateBasedPhiWidget::StateBasedPhiWidget(QWidget *parent) : AbstractAnalysisWidget(parent){
	QVBoxLayout *mainVerticalBox = new QVBoxLayout(this);

	//Create a state based phi analysis dao to be used by this class
	stateDao = new StateBasedPhiAnalysisDao(Globals::getAnalysisDao()->getDBInfo());

	//Initialize analysis parameters and other variables
	initializeAnalysisInfo();

	//Set up analysis runner with function to create threads to analyze for state-based phi
	analysisRunner->setTimeStepThreadCreationFunction(&createAnalysisTimeStepThread);

	//Add tool bar to top of widget
	toolBar = getDefaultToolBar();
	checkToolBarEnabled();//Can only carry out analysis if a network and archive are loaded
	mainVerticalBox->addWidget(toolBar);

	//Create a tabbed widget to hold progress and results
	QTabWidget* tabWidget = new QTabWidget(this);

	//Add the model and view displaying the current analysis
	fullResultsModel = new FullResultsModel(&analysisInfo, stateDao);
	QTableView* fullResultsTableView = new FullResultsTableView(this, fullResultsModel);
	fullResultsTableView->setMinimumSize(500, 500);
	tabWidget->addTab(fullResultsTableView, "Results");

	//Add widget displaying progress
	progressWidget = new ProgressWidget(this);
	connect(analysisRunner, SIGNAL(progress(const QString&, unsigned int, unsigned int, unsigned int)), progressWidget, SLOT(updateProgress(const QString&, unsigned int, unsigned int, unsigned int)), Qt::QueuedConnection);
	connect(analysisRunner, SIGNAL(timeStepComplete(unsigned int)), progressWidget, SLOT(timeStepComplete(unsigned int)), Qt::QueuedConnection);
	progressWidget->setMinimumSize(500, 500);
	QScrollArea* progressScrollArea = new QScrollArea(this);
	progressScrollArea->setWidget(progressWidget);
	tabWidget->addTab(progressScrollArea, "Progress");
	mainVerticalBox->addWidget(tabWidget);

	mainVerticalBox->addStretch(5);
}


/*! Destructor */
StateBasedPhiWidget::~StateBasedPhiWidget(){
	delete stateDao;
	delete fullResultsModel;
}


/*-----------------------------------------------------------*/
/*-------               PUBLIC METHODS                 ------*/
/*-----------------------------------------------------------*/

/*! Hides the analysis results - particularly the neurons highlighted in the network viewer */
void StateBasedPhiWidget::hideAnalysisResults(){
	fullResultsModel->hideAnalysisResults();
}


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

/*! Exports data from the analysis */
void StateBasedPhiWidget::exportAnalysis(){
	//Show dialog to select the analysis the user wants to load
	StateBasedPhiExportDialog exportAnalysisDialog(getAnalysisName(), this);
	exportAnalysisDialog.exec();
}


/*! Resets everything ready for a new analysis */
void StateBasedPhiWidget::newAnalysis(){
	//Set enabled status of toolbar
	checkToolBarEnabled();

	//Reset the analysis info
	initializeAnalysisInfo();

	//Clear analysis id
	Globals::setAnalysisID(getAnalysisName(), 0);

	//Reload model
	fullResultsModel->reload();

	//Reset progress widget
	progressWidget->reset();
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
		if(!Globals::isAnalysisLoaded(getAnalysisName())){
			Globals::getAnalysisDao()->addAnalysis(analysisInfo);

			//Analysis id in analysisInfo should now be set
			if(analysisInfo.getID() == 0){
				qCritical()<<"Analysis has not been added correctly";
				return;
			}

			//Store analysis ID in Globals to indicate that it is now loaded
			Globals::setAnalysisID(getAnalysisName(), analysisInfo.getID());
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
			stateDao->deleteTimeSteps(analysisInfo.getID(), firstTimeStep, lastTimeStep);
			updateResults();
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


/*! Updates results table by reloading it from the database */
void StateBasedPhiWidget::updateResults(){
	fullResultsModel->reload();
}


/*-------------------------------------------------------------*/
/*-------                PRIVATE METHODS                 ------*/
/*-------------------------------------------------------------*/

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
	analysisInfo.getParameterMap()["generalization"] = 1.0;
	analysisInfo.getParameterMap()["ignore_disconnected_subsets"] = 1.0;
	analysisInfo.getParameterMap()["minimum_complex_phi"] = 1.0;
}


/*! Checks to see if any of the specified range of time steps already exist in the database */
bool StateBasedPhiWidget::timeStepsAlreadyAnalyzed(int firstTimeStep, int lastTimeStep){
	if( !Globals::isAnalysisLoaded( getAnalysisName() ) ){
		return false;
	}

	int complexCount = stateDao->getComplexCount(analysisInfo.getID(), firstTimeStep, lastTimeStep);
	if(complexCount > 0)
		return true;
	return false;
}


