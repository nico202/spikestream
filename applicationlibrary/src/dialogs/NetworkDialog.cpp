#include "Globals.h"
#include "NetworkInfo.h"
#include "NetworkDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPushButton>


/*! Constructor called when creating a new network */
NetworkDialog::NetworkDialog(QWidget* parent) : QDialog(parent){
	addNetworkToDatabase = true;

	buildGUI("Unnamed", "Undescribed");
}


/*! Constructor called when altering name or description of a network. */
NetworkDialog::NetworkDialog(const QString& name, const QString& description, QWidget* parent) : QDialog(parent){
	addNetworkToDatabase = false;

	buildGUI(name, description);
}


/*! Destructor */
NetworkDialog::~NetworkDialog(){
}


/*! Returns the name of the network. */
QString NetworkDialog::getName(){
	QString nameStr = nameLineEdit->text();
	if(nameStr.isEmpty())
		nameStr = "Unnamed";
	return nameStr;
}


/*! Returns the description of the network. */
QString NetworkDialog::getDescription(){
	QString descStr = descLineEdit->text();
	if(descStr.isEmpty())
		descStr = "Undescribed";
	return descStr;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Rejects the dialog */
void NetworkDialog::cancelButtonPressed(){
	this->reject();
}


/*! Adds new network to the database, triggers reload and hides dialog */
void NetworkDialog::okButtonPressed(){
	if(addNetworkToDatabase){
		//Add the network
		NetworkInfo netInfo(0, getName(), getDescription());
		try{
			Globals::getNetworkDao()->addNetwork(netInfo);
			Globals::getEventRouter()->networkListChangedSlot();
		}
		catch(SpikeStreamException& ex){
			qCritical()<<ex.getMessage();
		}
	}

	//Hide the dialog
	this->accept();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Constructs the graphical interface */
void NetworkDialog::buildGUI(const QString& name, const QString& description){
	//Create layout managers
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(10);

	//Field to enter name of new network
	gridLayout->addWidget(new QLabel("Name: "), 0, 0);
	nameLineEdit = new QLineEdit(name);
	nameLineEdit->setMinimumSize(250, 30);
	gridLayout->addWidget(nameLineEdit, 0, 1);

	//Field to enter description of new network
	gridLayout->addWidget(new QLabel("Description: "), 1, 0);
	descLineEdit = new QLineEdit(description);
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

