#include "Globals.h"
#include "NetworkInfo.h"
#include "NewNetworkDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPushButton>


/*! Constructor */
NewNetworkDialog::NewNetworkDialog(QWidget* parent) : QDialog(parent){
	//Create layout managers
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(10);

	//Field to enter name of new network
	gridLayout->addWidget(new QLabel("Name: "), 0, 0);
	nameLineEdit = new QLineEdit("Unnamed");
	nameLineEdit->setMinimumSize(250, 30);
	gridLayout->addWidget(nameLineEdit, 0, 1);

	//Field to enter description of new network
	gridLayout->addWidget(new QLabel("Description: "), 1, 0);
	descLineEdit = new QLineEdit("Undescribed");
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


/*! Destructor */
NewNetworkDialog::~NewNetworkDialog(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Rejects the dialog */
void NewNetworkDialog::cancelButtonPressed(){
	this->reject();
}


/*! Adds new network to the database, triggers reload and hides dialog */
void NewNetworkDialog::okButtonPressed(){
	//Fix inputs
	QString nameStr = nameLineEdit->text();
	if(nameStr.isEmpty())
		nameStr = "Unnamed";
	QString descStr = descLineEdit->text();
	if(descStr.isEmpty())
		descStr = "Undescribed";

	//Add the network
	NetworkInfo netInfo(0, nameStr, descStr);
	try{
		Globals::getNetworkDao()->addNetwork(netInfo);
		Globals::getEventRouter()->reloadSlot();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}

	//Hide the dialog
	this->accept();
}



