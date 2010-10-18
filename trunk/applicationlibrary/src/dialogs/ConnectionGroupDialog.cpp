#include "Globals.h"
#include "ConnectionGroupDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPushButton>


/*! Constructor called when altering name or description of a network. */
ConnectionGroupDialog::ConnectionGroupDialog(const ConnectionGroupInfo& conGrpInfo, QWidget* parent) : QDialog(parent){
	connectionGroupInfo = conGrpInfo;
	buildGUI();
}


/*! Destructor */
ConnectionGroupDialog::~ConnectionGroupDialog(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Rejects the dialog */
void ConnectionGroupDialog::cancelButtonPressed(){
	this->reject();
}


/*! Adds new network to the database, triggers reload and hides dialog */
void ConnectionGroupDialog::okButtonPressed(){
	try{
		//Check network is loaded
		if(!Globals::networkLoaded())
			throw SpikeStreamException("Network not loaded when editing connection group properties.");

		//Set the new properties in the network if they have changed
		if(connectionGroupInfo.getDescription() != getDescription()){
			Globals::getNetwork()->setConnectionGroupProperties(connectionGroupInfo.getID(), getDescription());
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
void ConnectionGroupDialog::buildGUI(){
	//Create layout managers
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(10);

	//Field to enter description of connection group
	gridLayout->addWidget(new QLabel("Description: "), 1, 0);
	descLineEdit = new QLineEdit(connectionGroupInfo.getDescription());
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


/*! Returns the description of the network. */
QString ConnectionGroupDialog::getDescription(){
	QString descStr = descLineEdit->text();
	if(descStr.isEmpty())
		descStr = "Undescribed";
	return descStr;
}


