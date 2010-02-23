#include "SuccessWidget.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QPushButton>


/*! Constructor */
SuccessWidget::SuccessWidget(QWidget* parent) : QWidget(parent){
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	mainVBox->setMargin(10);

	//Add labels
	networkResultsLabel = new QLabel("");
	archiveResultsLabel = new QLabel("");
	analysisResultsLabel = new QLabel("");
	mainVBox->addWidget(networkResultsLabel);
	mainVBox->addWidget(archiveResultsLabel);
	mainVBox->addWidget(analysisResultsLabel);

	mainVBox->addStretch(5);

	//Add ok button
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	QPushButton* okButton = new QPushButton("Ok");
	connect(okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
	buttonLayout->addStretch(3);
	buttonLayout->addWidget(okButton);
	mainVBox->addLayout(buttonLayout);

}


/*! Destructor */
SuccessWidget::~SuccessWidget(){
}


/*! Sets the result labels appropriately */
void SuccessWidget::setResults(bool netDBConfigured, bool archDBConfigured, bool anaDBConfigured){
	//Network database results
	if(netDBConfigured){
		networkResultsLabel->setStyleSheet( "QLabel { color: #B60011; font-weight: bold; }");
		networkResultsLabel->setText("SpikeStreamNetwork database configured.");
	}
	else {
		networkResultsLabel->setStyleSheet( "QLabel { color: #000000; font-weight: normal;}");
		networkResultsLabel->setText("SpikeStreamNetwork database unchanged.");
	}

	//Archive database results
	if(archDBConfigured){
		archiveResultsLabel->setStyleSheet( "QLabel { color: #B60011; font-weight: bold;}");
		archiveResultsLabel->setText("SpikeStreamArchive database configured.");
	}
	else {
		archiveResultsLabel->setStyleSheet( "QLabel { color: #000000; font-weight: normal;}");
		archiveResultsLabel->setText("SpikeStreamArchive database unchanged.");
	}

	//Analysis database results
	if(anaDBConfigured){
		analysisResultsLabel->setStyleSheet( "QLabel { color: #B60011; font-weight: bold;}");
		analysisResultsLabel->setText("SpikeStreamAnalysis database configured.");
	}
	else {
		analysisResultsLabel->setStyleSheet( "QLabel { color: #000000; font-weight: normal;}");
		analysisResultsLabel->setText("SpikeStreamAnalysis database unchanged.");
	}
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Called when the ok button is clicked. Closes the application */
void SuccessWidget::okButtonClicked(){
	emit closed();
}




