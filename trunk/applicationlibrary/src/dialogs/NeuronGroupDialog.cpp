#include "Globals.h"
#include "NeuronGroupDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPushButton>


/*! Constructor called when altering name or description of a network. */
NeuronGroupDialog::NeuronGroupDialog(const NeuronGroupInfo& neurGrpInfo, QWidget* parent) : QDialog(parent){
	neuronGroupInfo = neurGrpInfo;
	buildGUI();
}


/*! Destructor */
NeuronGroupDialog::~NeuronGroupDialog(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Rejects the dialog */
void NeuronGroupDialog::cancelButtonPressed(){
	this->reject();
}


/*! Adds new network to the database, triggers reload and hides dialog */
void NeuronGroupDialog::okButtonPressed(){
	try{
		//Check network is loaded
		if(!Globals::networkLoaded())
			throw SpikeStreamException("Network not loaded when editing neuron group properties.");

		//Set the new properties in the network if they have changed
		if(neuronGroupInfo.getName() != getName() || neuronGroupInfo.getDescription() != getDescription()){
			Globals::getNetwork()->setNeuronGroupProperties(
				neuronGroupInfo.getID(), getName(), getDescription());
		}

		//Hide the dialog
		this->accept();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Constructs the graphical interface */
void NeuronGroupDialog::buildGUI(){
	//Create layout managers
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(10);

	//Field to enter name of new network
	gridLayout->addWidget(new QLabel("Name: "), 0, 0);
	nameLineEdit = new QLineEdit(neuronGroupInfo.getName());
	nameLineEdit->setMinimumSize(250, 30);
	gridLayout->addWidget(nameLineEdit, 0, 1);

	//Field to enter description of new network
	gridLayout->addWidget(new QLabel("Description: "), 1, 0);
	descLineEdit = new QLineEdit(neuronGroupInfo.getDescription());
	descLineEdit->setMinimumSize(250, 30);
	gridLayout->addWidget(descLineEdit, 1, 1);

	mainVBox->addLayout(gridLayout);

	//Add buttons
	QHBoxLayout* buttonBox = new QHBoxLayout();
	QPushButton* okButton = new QPushButton("Ok");
	connect(okButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	buttonBox->addWidget(okButton);
	QPushButton* cancelButton = new QPushButton("Cancel");
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
	buttonBox->addWidget(cancelButton);
	mainVBox->addLayout(buttonBox);
}



/*! Returns the name of the network. */
QString NeuronGroupDialog::getName(){
	QString nameStr = nameLineEdit->text();
	if(nameStr.isEmpty())
		nameStr = "Unnamed";
	return nameStr;
}


/*! Returns the description of the network. */
QString NeuronGroupDialog::getDescription(){
	QString descStr = descLineEdit->text();
	if(descStr.isEmpty())
		descStr = "Undescribed";
	return descStr;
}


