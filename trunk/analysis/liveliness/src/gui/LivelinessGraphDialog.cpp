//SpikeStream includes
#include "Globals.h"
#include "SpikeStreamException.h"
#include "LivelinessGraphDialog.h"
#include "LivelinessDao.h"
#include "LivelinessSpectrogramData.h"
using namespace spikestream;

//Qt includes
#include <QCheckBox>


/*! Constructor */
LivelinessGraphDialog::LivelinessGraphDialog(QWidget* parent, const AnalysisInfo& info) : AbstractGraphDialog(parent, info){
	//Create data
	spectrogramData = new LivelinessSpectrogramData(analysisInfo);

	//Add spectrogram
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	try{
		spectrogram = new AnalysisSpectrogram(this, spectrogramData);
		mainVBox->addWidget(spectrogram);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Spectrogram error: "<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred";
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
LivelinessGraphDialog::~LivelinessGraphDialog(){
	//FIXME: COPY FUNCTION IN DATA IS NOT A PROPER COPY, SO DATA IS DELETED AUTOMATICALLY BY THE GRAPH
}


/*! Adds available time steps to the time step combo */
void LivelinessGraphDialog::addTimeSteps(QComboBox* combo){
	LivelinessDao livelinessDao(Globals::getAnalysisDao()->getDBInfo());
	QList<Cluster> clusterList = livelinessDao.getClusters(analysisInfo.getID());

	//Add the "all" entry
	combo->addItem("All");

	bool firstTime = true;
	unsigned int oldTimeStep = 0;
	foreach(Cluster clstr, clusterList){
		if(firstTime){
			combo->addItem( QString::number(clstr.getTimeStep()) );
			oldTimeStep = clstr.getTimeStep();
			firstTime = false;
		}
		else if(oldTimeStep != clstr.getTimeStep()){
			combo->addItem( QString::number(clstr.getTimeStep()) );
			oldTimeStep = clstr.getTimeStep();
		}
	}
}
