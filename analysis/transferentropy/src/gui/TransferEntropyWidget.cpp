//SpikeStream includes
#include "ArchiveDao.h"
#include "Globals.h"
#include "GlobalVariables.h"
#include "SpikeStreamException.h"
#include "TransferEntropyWidget.h"
#include "TransferEntropyCalculator.h"
#include "TransferEntropyTimeStepThread.h"
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
	/*! Creates a TransferEntropyWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new TransferEntropyWidget();
	}

	/*! Returns a sensible name for this widget */
	QString getName(){
		return QString("Transfer Entropy Analysis");
	}
}


/*! Returns a class that is used by the analysis runner for the analysis */
AbstractAnalysisTimeStepThread* createAnalysisTimeStepThread(const DBInfo& netDBInfo, const DBInfo& archDBInfo, const DBInfo& anaDBInfo){
	return new TransferEntropyTimeStepThread(netDBInfo, archDBInfo, anaDBInfo);
}


/*! Constructor */
TransferEntropyWidget::TransferEntropyWidget(QWidget *parent) : AbstractAnalysisWidget(parent){
	QVBoxLayout *mainVBox = new QVBoxLayout(this);

	//Initialize analysis parameters and other variables
	initializeAnalysisInfo();

	//Add tool bar to top of widget
	toolBar = getDefaultToolBar();
	checkToolBarEnabled();//Can only carry out analysis if a network and archive are loaded
	mainVBox->addWidget(toolBar);

	//Add status display area
	statusTextEdit = new QTextEdit(this);
	statusTextEdit->setReadOnly(true);
	statusTextEdit->append("Hello world");
	mainVBox->addWidget(statusTextEdit);

	mainVBox->addStretch(5);

}


/*! Destructor */
TransferEntropyWidget::~TransferEntropyWidget(){
}


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

/*! Exports data from the analysis */
void TransferEntropyWidget::exportAnalysis(){
}


/*! Hides analysis results - typically called when switching analysis widget */
void TransferEntropyWidget::hideAnalysisResults(){
}


/*! Resets everything ready for a new analysis */
void TransferEntropyWidget::newAnalysis(){
	//Set enabled status of toolbar
	checkToolBarEnabled();

	//Reset the analysis info
	initializeAnalysisInfo();

	//Clear analysis id
	Globals::setAnalysisID(getAnalysisName(), 0);
}


void TransferEntropyWidget::plotGraphs(){
}



/*! Starts the analysis of the network for transfer entropy */
void TransferEntropyWidget::startAnalysis(){
	//Double check that both a network and an analysis are loaded
	if(!Globals::networkLoaded() || !Globals::archiveLoaded()){
		qCritical()<<"Network and/or archive not loaded - cannot start analysis.";
		return;
	}

	//Store parameters for the analysis
	storeParameters();

	//Get time steps to be analyzed
	unsigned firstTimeStep = getFirstTimeStep();
	unsigned lastTimeStep = getLastTimeStep();

	//Check time window is less than range of time steps
	if(timeWindow > firstTimeStep - lastTimeStep){
		qCritical()<<"Time window must be less than the range of time steps.";
	}

	try{
		statusTextEdit->append("Starting analysis.");
		ArchiveDao archiveDao(Globals::getArchiveDao()->getDBInfo());

		//Vectors holding neuron data
		vector<unsigned> fromNeuronData;
		vector<unsigned> toNeuronData;

		statusTextEdit->append("Loading neuron data.");
		for(unsigned timeStep = firstTimeStep; timeStep <= lastTimeStep; ++timeStep){
			QList<neurid_t> firingNeuronIDs = archiveDao.getFiringNeuronIDs(analysisInfo.getArchiveID(), timeStep);

			//Look for from and to neuron ids in the data for this time step
			bool fromIDFound = false, toIDFound = false;
			for(int i=0; i<firingNeuronIDs.size(); ++i){
				if(firingNeuronIDs.at(i) == fromNeuronID){
					fromNeuronData.push_back(1);
					fromIDFound = true;
				}
				else if(firingNeuronIDs.at(i) == toNeuronID){
					toNeuronData.push_back(1);
					toIDFound = true;
				}
			}

			//Add zeros if no firing activity is found for this time step
			if(!fromIDFound)
				fromNeuronData.push_back(0);
			if(!toIDFound)
				toNeuronData.push_back(0);
		}

		//Check vectors are the correct length
		if(fromNeuronData.size() != lastTimeStep - firstTimeStep + 1 || fromNeuronData.size() != toNeuronData.size()){
			qCritical()<<"Error loading neuron data - vectors are the incorrect length";
			return;
		}

		//Work through data and analyze it for transfer entropy
		statusTextEdit->append("Calculating transfer entropy.");
		TransferEntropyCalculator transferEntropyCalculator(k_param, l_param, timeWindow);
		double tmpTransEnt, total=0.0;
		unsigned count = 0;
		for(unsigned startTimeStep=0; startTimeStep<fromNeuronData.size()-timeWindow; ++startTimeStep){
			tmpTransEnt = transferEntropyCalculator.getTransferEntropy(startTimeStep, toNeuronData, fromNeuronData);
			total += tmpTransEnt;
			++count;
			statusTextEdit->append("Start time step: " + QString::number(startTimeStep) + "; Transfer entropy: " + QString::number(tmpTransEnt));
		}
		statusTextEdit->append("Analysis finished. Total transfer entropy: " + QString::number(total) + "; Average transfer entropy: " + QString::number(total/count));
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"Unknown exception occurred.";
	}
}


/*! Updates results table by reloading it from the database */
void TransferEntropyWidget::updateResults(){
}


/*-------------------------------------------------------------*/
/*-------                PROTECTED METHODS               ------*/
/*-------------------------------------------------------------*/

/*! Sets initial state of parameters for a state based phi analysis */
void TransferEntropyWidget::initializeAnalysisInfo(){
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

	//Set the type of analysis, 3 corresponds to a liveliness analysis
	//FIXME: ACCESS THIS USING A STRING NOT A DATABASE ID
	analysisInfo.setAnalysisType(0);

	//Set parameters
	analysisInfo.getParameterMap()["time_window"] = 100;
	analysisInfo.getParameterMap()["k"] = 5;
	analysisInfo.getParameterMap()["l"] = 5;
	analysisInfo.getParameterMap()["from_neuron_ID"] = 0;
	analysisInfo.getParameterMap()["to_neuron_ID"] = 0;
}


/*-------------------------------------------------------------*/
/*-------                 PRIVATE METHODS                ------*/
/*-------------------------------------------------------------*/

/*! Extracts parameters and checks that they are valid. */
void TransferEntropyWidget::storeParameters(){
	//From and to neuron ids.
	fromNeuronID = analysisInfo.getParameterMap()["from_neuron_ID"];
	toNeuronID = analysisInfo.getParameterMap()["to_neuron_ID"];
	if(fromNeuronID == 0 || toNeuronID == 0){
		qCritical()<<"From and/or to neuron id has not been set.";
		return;
	}
	if(fromNeuronID == toNeuronID){
		qCritical()<<"From neuron ID should not equal to neuron id.";
		return;
	}

	//Time window
	timeWindow = analysisInfo.getParameterMap()["time_window"];
	k_param = analysisInfo.getParameterMap()["k"];
	l_param = analysisInfo.getParameterMap()["l"];

	if(k_param < l_param)
		qCritical()<<"k_param must be greater than or equal to l_param";
	if(timeWindow <= k_param)
		qCritical()<<"TimeWindow must be greater than k_param";

}

