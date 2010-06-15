#include "DBDetailsWidget.h"
using namespace spikestream;

//SpikeStream includes
#include "ConfigLoader.h"
#include "DBInfo.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

/*! Constructor */
DBDetailsWidget::DBDetailsWidget(QWidget* parent) : QWidget(parent){
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	//Create a config loader so that we can load up existing settings
	ConfigLoader configLoader;

	//Add instructions at the top of the widget
	QString instructionStr = "Enter the host, username and password for the databases that you want to configure.\n";
	instructionStr += "These can be on the same or different machines.\n";
	instructionStr += "You must have MySQL running on the specified  hosts before running this tool.";
	mainVBox->addWidget(new QLabel(instructionStr));

	try{
		//Add SpikeSreamNetwork fields
		QGroupBox* networkGroupBox = new QGroupBox("");
		netChkBox = new QCheckBox("Configure SpikeStreamNetwork database");
		netChkBox->setChecked(false);
		connect(netChkBox, SIGNAL(stateChanged(int)), this, SLOT(enableNetworkDBConfiguration(int)));
		networkHost = new QLineEdit(configLoader.getParameter("spikeStreamNetworkHost"));
		networkUsername = new QLineEdit(configLoader.getParameter("spikeStreamNetworkUser"));
		networkPassword = new QLineEdit(configLoader.getParameter("spikeStreamNetworkPassword"));
		networkPassword->setEchoMode(QLineEdit::Password);
		QGridLayout* netGridLayout = new QGridLayout();
		netGridLayout->addWidget(netChkBox, 0, 0, 1, 2);
		netGridLayout->addWidget(new QLabel("Host "), 1, 0);
		netGridLayout->addWidget(networkHost, 1, 1);
		netGridLayout->addWidget(new QLabel("Username "), 2, 0);
		netGridLayout->addWidget(networkUsername, 2, 1);
		netGridLayout->addWidget(new QLabel("Password "), 3, 0);
		netGridLayout->addWidget(networkPassword, 3, 1);
		networkGroupBox->setLayout(netGridLayout);
		enableNetworkDBConfiguration(Qt::Unchecked);
		mainVBox->addWidget(networkGroupBox);

		//Add SpikeSreamArchive fields
		QGroupBox* archiveGroupBox = new QGroupBox("");
		archChkBox = new QCheckBox("Configure SpikeStreamArchive database");
		archChkBox->setChecked(false);
		connect(archChkBox, SIGNAL(stateChanged(int)), this, SLOT(enableArchiveDBConfiguration(int)));
		archiveHost = new QLineEdit(configLoader.getParameter("spikeStreamArchiveHost"));
		archiveUsername = new QLineEdit(configLoader.getParameter("spikeStreamArchiveUser"));
		archivePassword = new QLineEdit(configLoader.getParameter("spikeStreamArchivePassword"));
		archivePassword->setEchoMode(QLineEdit::Password);
		QGridLayout* archGridLayout = new QGridLayout();
		archGridLayout->addWidget(archChkBox, 0, 0, 1, 2);
		archGridLayout->addWidget(new QLabel("Host "), 1, 0);
		archGridLayout->addWidget(archiveHost, 1, 1);
		archGridLayout->addWidget(new QLabel("Username "), 2, 0);
		archGridLayout->addWidget(archiveUsername, 2, 1);
		archGridLayout->addWidget(new QLabel("Password "), 3, 0);
		archGridLayout->addWidget(archivePassword, 3, 1);
		archiveGroupBox->setLayout(archGridLayout);
		enableArchiveDBConfiguration(Qt::Unchecked);
		mainVBox->addWidget(archiveGroupBox);

		//Add SpikeSreamAnalysis fields
		QGroupBox* analysisGroupBox = new QGroupBox("");
		anaChkBox = new QCheckBox("Configure SpikeStreamAnalysis database");
		anaChkBox->setChecked(false);
		connect(anaChkBox, SIGNAL(stateChanged(int)), this, SLOT(enableAnalysisDBConfiguration(int)));
		analysisHost = new QLineEdit(configLoader.getParameter("spikeStreamAnalysisHost"));
		analysisUsername = new QLineEdit(configLoader.getParameter("spikeStreamAnalysisUser"));
		analysisPassword = new QLineEdit(configLoader.getParameter("spikeStreamAnalysisPassword"));
		analysisPassword->setEchoMode(QLineEdit::Password);
		QGridLayout* anaGridLayout = new QGridLayout();
		anaGridLayout->addWidget(anaChkBox, 0, 0, 1, 2);
		anaGridLayout->addWidget(new QLabel("Host "), 1, 0);
		anaGridLayout->addWidget(analysisHost, 1, 1);
		anaGridLayout->addWidget(new QLabel("Username "), 2, 0);
		anaGridLayout->addWidget(analysisUsername, 2, 1);
		anaGridLayout->addWidget(new QLabel("Password "), 3, 0);
		anaGridLayout->addWidget(analysisPassword, 3, 1);
		analysisGroupBox->setLayout(anaGridLayout);
		enableAnalysisDBConfiguration(Qt::Unchecked);
		mainVBox->addWidget(analysisGroupBox);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}

	//Buttons at bottom of dialog
	QHBoxLayout *buttonBox = new QHBoxLayout();
	QPushButton* cancelButton = new QPushButton("Cancel");
	buttonBox->addWidget(cancelButton);
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
	QPushButton* okButton = new QPushButton("Ok");
	buttonBox->addWidget(okButton);
	connect (okButton, SIGNAL(clicked()), this, SLOT(getDBInformation()));
	mainVBox->addLayout(buttonBox);
}


/*! Destructor */
DBDetailsWidget::~DBDetailsWidget(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Closes application */
void DBDetailsWidget::cancelButtonPressed(){
	emit cancel();
}


/*! Enables or disables the controls for setting the analysis information */
void DBDetailsWidget::enableAnalysisDBConfiguration(int state){
	if(state == Qt::Checked){
		analysisHost->setEnabled(true);
		analysisUsername->setEnabled(true);
		analysisPassword->setEnabled(true);
	}
	else{
		analysisHost->setEnabled(false);
		analysisUsername->setEnabled(false);
		analysisPassword->setEnabled(false);
	}
}


/*! Enables or disables the controls for setting the archive information */
void DBDetailsWidget::enableArchiveDBConfiguration(int state){
	if(state == Qt::Checked){
		archiveHost->setEnabled(true);
		archiveUsername->setEnabled(true);
		archivePassword->setEnabled(true);
	}
	else{
		archiveHost->setEnabled(false);
		archiveUsername->setEnabled(false);
		archivePassword->setEnabled(false);
	}
}


/*! Enables or disables the controls for setting the network information */
void DBDetailsWidget::enableNetworkDBConfiguration(int state){
	if(state == Qt::Checked) {
		networkHost->setEnabled(true);
		networkUsername->setEnabled(true);
		networkPassword->setEnabled(true);
	}
	else{
		networkHost->setEnabled(false);
		networkUsername->setEnabled(false);
		networkPassword->setEnabled(false);
	}
}

/*! Extracts the database information that has been entered and emits a signal with it */
void DBDetailsWidget::getDBInformation(){
	//Alert user if no databases have been selected
	if(!netChkBox->isChecked() && !archChkBox->isChecked() && !anaChkBox->isChecked()){
		qCritical()<<"No databases have been selected for configuration";
		return;
	}

	//Extract the database information
	DBInfo netDBInfo, archDBInfo, anaDBInfo;
	try{
		if(netChkBox->isChecked()){
			netDBInfo.setHost(getDBInfoString(networkHost));
			netDBInfo.setUser(getDBInfoString(networkUsername));
			netDBInfo.setPassword(getDBInfoString(networkPassword));
			netDBInfo.setDatabase("SpikeStreamNetwork");
		}
		if(archChkBox->isChecked()){
			archDBInfo.setHost(getDBInfoString(archiveHost));
			archDBInfo.setUser(getDBInfoString(archiveUsername));
			archDBInfo.setPassword(getDBInfoString(archivePassword));
			archDBInfo.setDatabase("SpikeStreamArchive");
		}
		if(anaChkBox->isChecked()){
			anaDBInfo.setHost(getDBInfoString(analysisHost));
			anaDBInfo.setUser(getDBInfoString(analysisUsername));
			anaDBInfo.setPassword(getDBInfoString(analysisPassword));
			anaDBInfo.setDatabase("SpikeStreamAnalysis");
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Information about the host, username or password is missing.";
		return;
	}

	//Signal with information to other classes
	emit dbInformationEntered(netDBInfo, archDBInfo, anaDBInfo);
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Returns the string in the line edit text field, or throws an
	exception if the string is empty */
QString DBDetailsWidget::getDBInfoString(QLineEdit* lineEdit){
	if(lineEdit->text().isEmpty())
		throw SpikeStreamException("Field contains empty string.");
	return lineEdit->text();
}

