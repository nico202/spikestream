//SpikeStream includes
#include "TruthTableDialog.h"
#include "TruthTableView.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLayout>
#include <QPushButton>


/*! Constructor */
TruthTableDialog::TruthTableDialog(QWidget* parent) : QDialog(parent) {
	QVBoxLayout *mainVBox = new QVBoxLayout(this);

	//Create model and add view
	truthTableModel = new TruthTableModel();
	QTableView* truthTableView = new TruthTableView(truthTableModel);
	truthTableView->setMinimumSize(600, 300);
	mainVBox->addWidget(truthTableView);

	//Add buttons
	QHBoxLayout* buttonBox = new QHBoxLayout();
	QPushButton* okPushButton = new QPushButton("Ok");
	buttonBox->addWidget(okPushButton);
	QPushButton* cancelPushButton = new QPushButton("Cancel");
	buttonBox->addWidget(cancelPushButton);
	mainVBox->addLayout(buttonBox);

	connect (okPushButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	connect (cancelPushButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
}


/*! Destructor */
TruthTableDialog::~TruthTableDialog(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Shows the dialog with the truth table for the specified neuron id */
void TruthTableDialog::show(unsigned int neuronID){
	this->setWindowTitle("Truth Table for Neuron " + QString::number(neuronID));
	truthTableModel->setNeuronID(neuronID);
	this->setVisible(true);
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Hides the dialog when the cancel button is pressed. */
void TruthTableDialog::cancelButtonPressed(){
	this->hide();
}


/*! Hides the dialog when the ok button is pressed. */
void TruthTableDialog::okButtonPressed(){
	this->hide();
}

