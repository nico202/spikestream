//SpikeStream includes
#include "AbstractGraphDialog.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QCheckBox>
#include <QDebug>
#include <QLayout>
#include <QLabel>


/*! Constructor */
AbstractGraphDialog::AbstractGraphDialog(QWidget* parent, const AnalysisInfo& anaInfo) : QDialog(parent){
	//Store references
	this->analysisInfo = anaInfo;

	this->setStyleSheet("QWidget { background-color: white; font-family: Arial; font-size: 25px; } ");
}


/*! Destructor */
AbstractGraphDialog::~AbstractGraphDialog(){
}


/*----------------------------------------------------------*/
/*------              PROTECTED SLOTS                 ------*/
/*----------------------------------------------------------*/

/*! Controls whether neuron ids or numbers starting from 1 are shown */
void AbstractGraphDialog::showNeuronIDS(int state){
	if(state == Qt::Checked)
		spectrogramData->setShowNeuronIDs(true);
	else
		spectrogramData->setShowNeuronIDs(false);
	spectrogram->rescale();
	spectrogram->replot();
}


/*! Called when the time step in the combo has changed */
void AbstractGraphDialog::timeStepChanged(QString timeStepStr){
	if(timeStepStr == "All")
		spectrogramData->showAllTimeSteps(true);
	else{
		spectrogramData->showAllTimeSteps(false);
		spectrogramData->showSelectedTimeStep(Util::getUInt(timeStepStr));
	}
	spectrogram->replot();
}


