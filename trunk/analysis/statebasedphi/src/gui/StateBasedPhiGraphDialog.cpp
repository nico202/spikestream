//SpikeStream includes
#include "Globals.h"
#include "SpikeStreamException.h"
#include "StateBasedPhiGraphDialog.h"
#include "StateBasedPhiAnalysisDao.h"
#include "StateBasedPhiSpectrogramData.h"
using namespace spikestream;

//Qt includes
#include <QCheckBox>


/*! Constructor */
StateBasedPhiGraphDialog::StateBasedPhiGraphDialog(QWidget* parent, const AnalysisInfo& info) : AbstractGraphDialog(parent, info){
	//Create data
	spectrogramData = new StateBasedPhiSpectrogramData(analysisInfo);

	//Add spectrogram
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	try{
		spectrogram = new AnalysisSpectrogram(this, spectrogramData);
		mainVBox->addWidget(spectrogram);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Spectrogram error: "<<ex.getMessage();
	}

	//Add controls to select time step
	QHBoxLayout* controlsLayout = new QHBoxLayout();
	QComboBox* timeStepCombo = new QComboBox();
	addTimeSteps(timeStepCombo);
	connect(timeStepCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(timeStepChanged(QString)));
	controlsLayout->addWidget(new QLabel("Time step: "));
	controlsLayout->addWidget(timeStepCombo);

	//Add control to view the neuron ids or neurons numbered 1, 2, ...
	QCheckBox* neurIDChkBox = new QCheckBox("Show Neuron IDs");
	neurIDChkBox->setChecked(true);
	connect(neurIDChkBox, SIGNAL(stateChanged(int)), this, SLOT(showNeuronIDS(int)));
	controlsLayout->addWidget(neurIDChkBox);
	controlsLayout->addStretch(5);
	mainVBox->addLayout(controlsLayout);
}


/*! Destructor */
StateBasedPhiGraphDialog::~StateBasedPhiGraphDialog(){
	//FIXME: COPY FUNCTION IN DATA IS NOT A PROPER COPY, SO DATA IS DELETED AUTOMATICALLY BY THE GRAPH
}


/*! Adds available time steps to the time step combo */
void StateBasedPhiGraphDialog::addTimeSteps(QComboBox* combo){
	StateBasedPhiAnalysisDao stateDao(Globals::getAnalysisDao()->getDBInfo());
	QList<Complex> complexList = stateDao.getComplexes(analysisInfo.getID());

	//Add the "all" entry
	combo->addItem("All");

	bool firstTime = true;
	unsigned int oldTimeStep = 0;
	foreach(Complex cmplx, complexList){
		if(firstTime){
			combo->addItem( QString::number(cmplx.getTimeStep()) );
			oldTimeStep = cmplx.getTimeStep();
			firstTime = false;
		}
		else if(oldTimeStep != cmplx.getTimeStep()){
			combo->addItem( QString::number(cmplx.getTimeStep()) );
			oldTimeStep = cmplx.getTimeStep();
		}
	}
}
