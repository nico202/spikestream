//SpikeStream includes
#include "Globals.h"
#include "LoadAnalysisDialog.h"
#include "SpikeStreamException.h"
#include "StateBasedPhiWidget.h"
using namespace spikestream;

//Qt includes
#include <QAction>
#include <QLabel>
#include <QLayout>
#include <QDebug>
#include <QIcon>
#include <QSqlQueryModel>

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
    toolBar = getToolBar();

    //Can only carry out analysis if a network and archive are loaded
    checkToolBarEnabled();

    mainVerticalBox->addWidget(toolBar);

    //Add the table view displaying the current analysis
    analysisDataTableView = new QTableView();
    analysisDataTableView->setMinimumSize(500, 300);
    mainVerticalBox->addWidget(analysisDataTableView);

    //Create a state based phi analysis dao to be used by this class
    stateDao = new StateBasedPhiAnalysisDao(Globals::getAnalysisDao()->getDBInfo());

    //Listen for events that affect whether the tool bar should be enabled or not.
    connect(Globals::getEventRouter(), SIGNAL(archiveChangedSignal()), this, SLOT(checkToolBarEnabled()));
    connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(checkToolBarEnabled()));
}


/*! Destructor */
StateBasedPhiWidget::~StateBasedPhiWidget(){
    //qDebug()<<"DestroyingStateBasedPhiWidget";
}


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

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


/*! Displays the load analysis dialog box so the user can select which analysis to load */
void StateBasedPhiWidget::loadAnalysis(){
    try{
	//Show dialog to select the analysis the user wants to load
	LoadAnalysisDialog loadAnalysisDialog(this);
	if(loadAnalysisDialog.exec() == QDialog::Accepted ) {//Load the archive
	    analysisInfo = loadAnalysisDialog.getAnalysisInfo();

	    //Get the model with details about the selected analysis from the analysis table and display it in the view
	    QSqlQueryModel* model = stateDao->getStateBasedPhiDataTableModel(analysisInfo.getID());
	    analysisDataTableView->setModel(model);
	    analysisDataTableView->show();

	    //Get the minimum and maximum time step values and load them into the combo box
	    unsigned int minTimeStep = Globals::getArchiveDao()->getMinTimeStep(Globals::getArchive()->getID());
	    unsigned int maxTimeStep = Globals::getArchiveDao()->getMaxTimeStep(Globals::getArchive()->getID());
	    QStringList timeStepList = getTimeStepList(minTimeStep, maxTimeStep);
	    fromTimeStepCombo->clear();
	    fromTimeStepCombo->addItems(timeStepList);
	    toTimeStepCombo->clear();
	    toTimeStepCombo->addItems(timeStepList);

	    //Record fact that analysis is loaded
	}
    }
    catch (SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
	return;
    }
}


/*! Resets everything ready for a new analysis */
void StateBasedPhiWidget::newAnalysis(){
}


/*! Checks to see if network or archive have been loaded */
void StateBasedPhiWidget::checkToolBarEnabled(){
    if(Globals::networkLoaded() && Globals::archiveLoaded())
	toolBar->setEnabled(true);
    else
	toolBar->setEnabled(false);
}


/*! Selects the parameters to be used for the analysis, including the description.
    These cannot be edited once the analysis has been started - otherwise would have to associate
    a set of parameter with each time step */
void StateBasedPhiWidget::selectParameters(){
    //StateBasedPhiParameterDialog dialog(analysisDescription, parameterMap);
}


/*! Starts the analysis of the network for state-based phi */
void StateBasedPhiWidget::startAnalysis(){
    //Check to see if the selected time steps are fully or partially present in the database

    //Create a new analysis in the database if one is not loaded
    if(!analysisLoaded()){
//	Globals::getAnalysisDao()->addAnalysis(analysisInfo);
    }
}


/*! Stops the analysis of the network. */
void StateBasedPhiWidget::stopAnalysis(){

}


/*-------------------------------------------------------------*/
/*-------                PRIVATE METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Returns true if we are working with an analysis that already exists in the database */
bool StateBasedPhiWidget::analysisLoaded(){
    if(analysisInfo.getID() == 0)
	return false;
    return true;
}


QStringList StateBasedPhiWidget::getTimeStepList(unsigned int min, unsigned int max){
    QStringList tmpStrList;
    for(unsigned int i=min; i<max; ++i)
	tmpStrList.append(QString::number(i));
    return tmpStrList;
}


/*! Builds the toolbar that goes at the top of the page. */
QToolBar* StateBasedPhiWidget::getToolBar(){
    QToolBar* tmpToolBar = new QToolBar(this);

    QAction* tmpAction = new QAction(QIcon(), "Open", this);
    connect(tmpAction, SIGNAL(triggered()), this, SLOT(loadAnalysis()));
    tmpToolBar->addAction (tmpAction);

    tmpAction = new QAction(QIcon(), "New", this);
    connect(tmpAction, SIGNAL(triggered()), this, SLOT(newAnalysis()));
    tmpToolBar->addAction (tmpAction);

    tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/play.xpm"), "Start analysis", this);
    connect(tmpAction, SIGNAL(triggered()), this, SLOT(startAnalysis()));
    tmpToolBar->addAction (tmpAction);

    tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/stop.xpm"), "Stop analysis", this);
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




