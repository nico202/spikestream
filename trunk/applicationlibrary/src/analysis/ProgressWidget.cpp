#include "ProgressWidget.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QMutexLocker>


/*! Constructor */
ProgressWidget::ProgressWidget(QWidget* parent) : QWidget(parent) {
	QVBoxLayout* mainVerticalBox = new QVBoxLayout(this);
	gridLayout = new QGridLayout();
	mainVerticalBox->addLayout(gridLayout);
	mainVerticalBox->addStretch(5);
}


/*! Destructor */
ProgressWidget::~ProgressWidget(){
}


/*-------------------------------------------------------------*/
/*-------                 PUBLIC METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Clears all of the progress indicators */
void ProgressWidget::reset(){
	for(int row=0; row<gridLayout->rowCount(); ++row){
		QLayoutItem* label = gridLayout->itemAtPosition(row, 0);
		if(label !=0){
			label->widget()->deleteLater();
		}
		QLayoutItem* bar = gridLayout->itemAtPosition(row, 1);
		if(bar !=0){
			bar->widget()->deleteLater();
		}
		QLayoutItem* details = gridLayout->itemAtPosition(row, 2);
		if(details !=0){
			details->widget()->deleteLater();
		}
	}
	progressBarMap.clear();
	progressDetailsMap.clear();
}


/*-------------------------------------------------------------*/
/*-------                 PRIVATE SLOTS                  ------*/
/*-------------------------------------------------------------*/

/*! Removes progress information for the specified time step */
void ProgressWidget::timeStepComplete(unsigned int timeStep){
	QMutexLocker locker(&mutex);

	if(!progressBarMap.contains(timeStep)){
		qCritical()<<"Time step "<<timeStep<<" is claimed to be complete, but no time step found.";
		return;
	}
}


/*! Update the progress bar corresponding to the time step
	When the number of time steps completed matches the total number of time steps,
	the progress bar is removed. */
void ProgressWidget::updateProgress(const QString& msg, unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps){
	QMutexLocker locker(&mutex);

	//Check to see if we have a progress bar for this time step
	if(progressBarMap.contains(timeStep)){
		progressBarMap[timeStep]->setMaximum(totalSteps);
		progressBarMap[timeStep]->setValue(stepsCompleted);
		progressDetailsMap[timeStep]->setText(msg);
	}
	//Add new progress bar
	else{
		addProgressBar(timeStep, stepsCompleted, totalSteps);
	}
}


/*-------------------------------------------------------------*/
/*-------                PRIVATE METHODS                 ------*/
/*-------------------------------------------------------------*/

/*! Adds a progress bar to the bottom of the layout */
void ProgressWidget::addProgressBar(unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps){
	if(progressBarMap.contains(timeStep)){
		qCritical()<<"Attempting to add a progress bar for time step "<<timeStep<<" that already exists.";
		return;
	}

	//Progress bar
	QProgressBar* progBar = new QProgressBar();
	progBar->setMinimumSize(100, 15);
	progBar->setBaseSize(100, 15);
	progBar->setRange(0, totalSteps);
	progBar->setValue(stepsCompleted);
	gridLayout->addWidget(new QLabel("Time step " + QString::number(timeStep) + ": "), progressBarMap.size(), 0);
	gridLayout->addWidget(progBar, progressBarMap.size(), 1);
	progressBarMap[timeStep] = progBar;

	//Progress details
	QLabel* progDetails = new QLabel();
	progDetails->setMinimumSize(300, 15);
	progDetails->setBaseSize(300, 15);
	gridLayout->addWidget(progDetails, progressDetailsMap.size(), 2);
	progressDetailsMap[timeStep] = progDetails;
}

