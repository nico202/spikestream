//SpikeStream includes
#include "DescriptionDialog.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPushButton>


/*! Constructor. */
DescriptionDialog::DescriptionDialog(const QString& description, QWidget* parent) : QDialog(parent){
	buildGUI(description);
}


/*! Destructor */
DescriptionDialog::~DescriptionDialog(){
}


/*! Returns the description of the network. */
QString DescriptionDialog::getDescription(){
	QString descStr = descLineEdit->text();
	if(descStr.isEmpty())
		descStr = "Undescribed";
	return descStr;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Rejects the dialog */
void DescriptionDialog::cancelButtonPressed(){
	this->reject();
}


/*! Accepts the dialog */
void DescriptionDialog::okButtonPressed(){
	this->accept();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Constructs the graphical interface */
void DescriptionDialog::buildGUI(const QString& description){
	//Create layout managers
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(10);

	//Field to enter description
	gridLayout->addWidget(new QLabel("Description: "), 0, 0);
	descLineEdit = new QLineEdit(description);
	descLineEdit->setMinimumSize(250, 30);
	gridLayout->addWidget(descLineEdit, 0, 1);

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

