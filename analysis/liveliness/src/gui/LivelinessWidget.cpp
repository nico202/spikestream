//SpikeStream includes
#include "LivelinessExportDialog.h"
#include "LivelinessFullResultsTableView.h"
#include "Globals.h"
#include "GlobalVariables.h"
#include "SpikeStreamException.h"
#include "LivelinessWidget.h"
#include "LivelinessTimeStepThread.h"
#include "LivelinessGraphDialog.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>
#include <QDebug>
#include <QScrollArea>
#include <QTabWidget>
#include <QMessageBox>

//Other includes
#include <iostream>
using namespace std;


//Functions for dynamic library loading
extern "C" {
	/*! Creates a LivelinessWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new LivelinessWidget();
	}

	/*! Returns a sensible name for this widget */
	QString getName(){
		return QString("Liveliness Analysis");
	}
}


/*! Returns a class that is used by the analysis runner for the analysis */
AbstractAnalysisTimeStepThread* createAnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo){
	return new LivelinessTimeStepThread(netDBInfo, archDBInfo, anaDBInfo);
}


/*! Constructor */
LivelinessWidget::LivelinessWidget(QWidget *parent) : AbstractAnalysisWidget(parent){
	QVBoxLayout *mainVerticalBox = new QVBoxLayout(this);

	//Create a liveliness dao to be used by this class
	livelinessDao = new LivelinessDao(Globals::getAnalysisDao()->getDBInfo());

	//Initialize analysis parameters and other variables
	initializeAnalysisInfo();

	//Set up analysis runner with function to create threads to analyze for liveliness
	analysisRunner->setTimeStepThreadCreationFunction(&createAnalysisTimeStepThread);

	//Add tool bar to top of widget
	toolBar = getDefaultToolBar();
	checkToolBarEnabled();//Can only carry out analysis if a network and archive are loaded
	mainVerticalBox->addWidget(toolBar);

	//Create a tabbed widget to hold progress and results
	QTabWidget* tabWidget = new QTabWidget(this);

	//Add the model and view displaying the current analysis
	fullResultsModel = new LivelinessFullResultsModel(&analysisInfo, livelinessDao);
	QTableView* fullResultsTableView = new LivelinessFullResultsTableView(this, fullResultsModel);
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

	//Add heat color bar and controls for setting the range
	QHBoxLayout* heatLayout = new QHBoxLayout();
	heatColorBar = new HeatColorBar(this);
	heatLayout->addWidget(heatColorBar);
	QValidator *validator = new QIntValidator(1, 100000, this);
	heatRangeEdit = new QLineEdit(QString::number(heatColorBar->getMaxValue()));
	heatRangeEdit->setValidator(validator);
	heatLayout->addWidget(heatRangeEdit);
	QPushButton* setMaxButton = new QPushButton("Set Max");
	connect(setMaxButton, SIGNAL(clicked()), this, SLOT(setHeatColorRangeMax()));
	heatLayout->addWidget(setMaxButton);
	QPushButton* resetButton = new QPushButton("Reset");
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetHeatColorRange()));
	heatLayout->addWidget(resetButton);
	mainVerticalBox->addLayout(heatLayout);

	mainVerticalBox->addStretch(5);

}


/*! Destructor */
LivelinessWidget::~LivelinessWidget(){
	delete livelinessDao;
	delete fullResultsModel;
}


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

/*! Exports data from the analysis */
void LivelinessWidget::exportAnalysis(){
	//Show dialog to select the analysis the user wants to load
	LivelinessExportDialog exportDialog(this);
	exportDialog.exec();
}


/*! Hides analysis results - typically called when switching analysis widget */
void LivelinessWidget::hideAnalysisResults(){
	fullResultsModel->hideClusters();
}


/*! Resets everything ready for a new analysis */
void LivelinessWidget::newAnalysis(){
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


void LivelinessWidget::plotGraphs(){
	LivelinessGraphDialog dialog(this, analysisInfo);
	dialog.exec();
}

/*! Resets the colour range to the value stored in the full results model.
	This is either the default, or the maximum neuron liveliness */
void LivelinessWidget::resetHeatColorRange(){
	if( !Globals::isAnalysisLoaded(getAnalysisName()) ){
		heatColorBar->setMaxValue( DEFAULT_MAX_HEAT_COLOR_VALUE );
		fullResultsModel->setMaxHeatColorValue(DEFAULT_MAX_HEAT_COLOR_VALUE);
		heatRangeEdit->setText(QString::number(DEFAULT_MAX_HEAT_COLOR_VALUE));
		return;
	}

	//Load maximum value from the database
	double newMaxHeatColorValue = DEFAULT_MAX_HEAT_COLOR_VALUE;
	try{
		newMaxHeatColorValue = livelinessDao->getMaxNeuronLiveliness(analysisInfo.getID());
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}

	//Set the new value
	heatColorBar->setMaxValue(newMaxHeatColorValue);
	fullResultsModel->setMaxHeatColorValue(newMaxHeatColorValue);
	heatRangeEdit->setText(QString::number(newMaxHeatColorValue));
}


void LivelinessWidget::setHeatColorRangeMax(){
	if(heatRangeEdit->text().isEmpty()){
		QMessageBox::warning(
				this,
				"SpikeStream Analysis", "No maximum range value has been specified.\nPleas enter a value.",
				QMessageBox::Ok
		);
		return;
	}
	double tmpMaxVal =Util::getDouble(heatRangeEdit->text());
	heatColorBar->setMaxValue(tmpMaxVal);
	fullResultsModel->setMaxHeatColorValue(tmpMaxVal);
}


/*! Starts the analysis of the network for state-based phi */
void LivelinessWidget::startAnalysis(){
	//Double check that both a network and an analysis are loaded
	if(!Globals::networkLoaded() || !Globals::archiveLoaded()){
		qCritical()<<"Network and/or archive not loaded - cannot start analysis.";
		return;
	}

	//Get time steps to be analyzed
	int firstTimeStep = getFirstTimeStep();
	int lastTimeStep = getLastTimeStep();

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
			livelinessDao->deleteTimeSteps(analysisInfo.getID(), firstTimeStep, lastTimeStep);
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
void LivelinessWidget::updateResults(){
	fullResultsModel->reload();
	resetHeatColorRange();
}


/*-------------------------------------------------------------*/
/*-------                PRIVATE METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Sets initial state of parameters for a state based phi analysis */
void LivelinessWidget::initializeAnalysisInfo(){
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

	//Set the type of analysis, 2 corresponds to a liveliness analysis
	//FIXME: ACCESS THIS USING A STRING NOT A DATABASE ID
	analysisInfo.setAnalysisType(2);

	//Set parameters
	analysisInfo.getParameterMap()["generalization"] = 1.0;
	analysisInfo.getParameterMap()["store_connection_liveliness_as_temporary_weights"] = 1.0;
	analysisInfo.getParameterMap()["minimum_cluster_liveliness"] = 1.0;
}


/*! Checks to see if any of the specified range of time steps already exist in the database */
bool LivelinessWidget::timeStepsAlreadyAnalyzed(int firstTimeStep, int lastTimeStep){
	if(!Globals::isAnalysisLoaded(getAnalysisName())){
		return false;
	}
	return livelinessDao->containsAnalysisData(analysisInfo.getID(), firstTimeStep, lastTimeStep);
}


