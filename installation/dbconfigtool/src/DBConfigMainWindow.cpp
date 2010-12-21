#include "DatabaseDao.h"
#include "AnalysisDao.h"
#include "DBConfigMainWindow.h"
#include "DBDetailsWidget.h"
#include "ConfigEditor.h"
#include "SuccessWidget.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLayout>
#include <QMessageBox>
#include <QProgressDialog>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
DBConfigMainWindow::DBConfigMainWindow(QWidget* parent) : QMainWindow(parent) {
	rootDirectory = Util::getRootDirectory();

	//Add a stacked widget with the pages
	QWidget* dbDetailsWidget = new DBDetailsWidget();
	connect(dbDetailsWidget, SIGNAL( dbInformationEntered(const DBInfo&, const DBInfo&, const DBInfo&) ), this, SLOT( configureDatabases(const DBInfo&, const DBInfo&, const DBInfo&) ) );
	connect(dbDetailsWidget, SIGNAL( cancel() ), this, SLOT( closeApplication() ) );
	successWidget = new SuccessWidget();
	connect(successWidget, SIGNAL( closed() ), this, SLOT( closeApplication() ) );
	stackedWidget = new QStackedWidget;
	stackedWidget->addWidget(dbDetailsWidget);
	stackedWidget->addWidget(successWidget);

	setCentralWidget(stackedWidget);
}


/*! Destructor */
DBConfigMainWindow::~DBConfigMainWindow(){
}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Closes the application */
void DBConfigMainWindow::closeApplication(){
	this->close();
}


/*! Configures each database, checking for confirmation from the user if the database already exists */
void DBConfigMainWindow::configureDatabases(const DBInfo& networkDBInfo, const DBInfo& archiveDBInfo, const DBInfo& analysisDBInfo){
	//Show busy window
	QProgressDialog progressDlg("Configuring databases...", "Abort", 0, 3, this);
	progressDlg.setWindowModality(Qt::WindowModal);
	progressDlg.setMinimumDuration(50);
	progressDlg.show();

	//Configure databases
	bool netDBConfigured = false, archDBConfigured = false, anaDBConfigured = false;
	try{
		//Configure SpikeStreamNetwork database
		if(networkDBInfo.getHost() != DBInfo::UNDEFINED){
			if(!addNetworkDatabases(networkDBInfo) || progressDlg.wasCanceled())
				return;
			netDBConfigured = true;
		}
		progressDlg.setValue(1);

		//Configure SpikeStreamArchive database
		if(archiveDBInfo.getHost() != DBInfo::UNDEFINED){
			if(!addArchiveDatabases(archiveDBInfo) || progressDlg.wasCanceled())
				return;
			archDBConfigured = true;
		}
		progressDlg.setValue(2);

		//Configure SpikeStreamAnalysis database
		if(analysisDBInfo.getHost() != DBInfo::UNDEFINED){
			if(!addAnalysisDatabases(analysisDBInfo) || progressDlg.wasCanceled())
				return;
			anaDBConfigured = true;
		}
		progressDlg.setValue(3);

		//Return if no databases have been configured
		if(!netDBConfigured && !archDBConfigured && !anaDBConfigured)
			return;
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		return;
	}

	//Write information to configuration file
	try{
		writeDBInfoToConfigFile(netDBConfigured, networkDBInfo, archDBConfigured, archiveDBInfo, anaDBConfigured, analysisDBInfo);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		return;
	}

	//Show success page
	successWidget->setResults(netDBConfigured, archDBConfigured, anaDBConfigured);
	stackedWidget->setCurrentIndex(1);
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Configures the SpikeStreamNetwork database using SQL files */
bool DBConfigMainWindow::addNetworkDatabases(const DBInfo& networkDBInfo){
	/* Create a copy of dbInfo without database entry in case the database has not been created
		Need to use a copy to save messing around with const_cast. */
	DBInfo tmpDBInfo = networkDBInfo;
	tmpDBInfo.setDatabase("");

	//Try and connect to database
	DatabaseDao dbDao;
	try{
		dbDao.connectToDatabase(tmpDBInfo);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Cannot connect to the host for SpikeStreamNetwork " + tmpDBInfo.toString();
		return false;
	}

	//Get a list of the current databases
	QList<QString> dbList = dbDao.getDatabaseNames();

	//Check that user wants to overwrite database if it already exists
	foreach (QString dbName, dbList){
		if(dbName.toLower() == "spikestreamnetwork"){
			bool ok = showConfirmMessage("A SpikeStreamNetwork database already exists at this location. Would you like to reset it?\nAll data will be lost. This step cannot be undone.");
			if(!ok)
				return false;
			break;
		}
	}

	//Add network and test databases
	dbDao.executeSQLFile(rootDirectory + "/database/SpikeStreamNetwork.sql");
	dbDao.executeSQLFile(rootDirectory + "/database/SpikeStreamNetworkTest.sql");
	dbDao.executeSQLFile("SpikeStreamNetwork", rootDirectory + "/database/AddNeuronTypes.sql");
	dbDao.executeSQLFile("SpikeStreamNetworkTest", rootDirectory + "/database/AddNeuronTypes.sql" );
	dbDao.executeSQLFile("SpikeStreamNetwork", rootDirectory + "/database/AddSynapseTypes.sql");
	dbDao.executeSQLFile("SpikeStreamNetworkTest", rootDirectory + "/database/AddSynapseTypes.sql" );

	//Operation completed successfully
	return true;
}


/*! Configures the SpikeStreamArchive database using SQL files */
bool DBConfigMainWindow::addArchiveDatabases(const DBInfo& archiveDBInfo){
	/* Create a copy of dbInfo without database entry in case the database has not been created
		Need to use a copy to save messing around with const_cast. */
	DBInfo tmpDBInfo = archiveDBInfo;
	tmpDBInfo.setDatabase("");

	//Try and connect to database
	DatabaseDao dbDao;
	try{
		dbDao.connectToDatabase(tmpDBInfo);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Cannot connect to the host for SpikeStreamArchive " + tmpDBInfo.toString();
		return false;
	}

	//Get a list of the current databases
	QList<QString> dbList = dbDao.getDatabaseNames();

	//Check that user wants to overwrite database if it already exists
	foreach (QString dbName, dbList){
		if(dbName.toLower() == "spikestreamarchive"){
			bool ok = showConfirmMessage("A SpikeStreamArchive database already exists at this location. Would you like to reset it?\nAll data will be lost. This step cannot be undone.");
			if(!ok)
				return false;
			break;
		}
	}

	//Add archive and test databases
	dbDao.executeSQLFile(rootDirectory + "/database/SpikeStreamArchive.sql");
	dbDao.executeSQLFile(rootDirectory + "/database/SpikeStreamArchiveTest.sql");

	//Operation completed successfully
	return true;
}


/*! Configures the SpikeStreamAnalysis database using SQL files */
bool DBConfigMainWindow::addAnalysisDatabases(const DBInfo& analysisDBInfo){
	/* Create a copy of dbInfo without database entry in case the database has not been created
		Need to use a copy to save messing around with const_cast. */
	DBInfo tmpDBInfo = analysisDBInfo;
	tmpDBInfo.setDatabase("");

	//Try and connect to database
	DatabaseDao dbDao;
	try{
		dbDao.connectToDatabase(tmpDBInfo);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Cannot connect to the host for SpikeStreamAnalysis " + tmpDBInfo.toString();
		return false;
	}

	//Get a list of the current databases
	QList<QString> dbList = dbDao.getDatabaseNames();

	//Check that user wants to overwrite database if it already exists
	foreach (QString dbName, dbList){
		if(dbName.toLower() == "spikestreamanalysis"){
			bool ok = showConfirmMessage("A SpikeStreamAnalysis database already exists at this location. Would you like to reset it?\nAll data will be lost. This step cannot be undone.");
			if(!ok)
				return false;
			break;
		}
	}

	//Add archive and test databases
	dbDao.executeSQLFile(rootDirectory + "/database/SpikeStreamAnalysis.sql");
	dbDao.executeSQLFile(rootDirectory + "/database/SpikeStreamAnalysisTest.sql");

	//Operation completed successfully
	return true;
}


/*! Stores the configuration information that has been entered in the configuration file. */
void DBConfigMainWindow::writeDBInfoToConfigFile(bool writeNetDB, const DBInfo& networkDBInfo, bool writeArchDB,  const DBInfo& archiveDBInfo, bool writeAnaDB, const DBInfo& analysisDBInfo){
	//Create a hash map with the parameters that are to be written
	QHash<QString, QString> configParams;

	//Add information if appropriate
	if(writeNetDB){
		configParams["spikeStreamNetworkHost"] = networkDBInfo.getHost();
		configParams["spikeStreamNetworkUser"] = networkDBInfo.getUser();
		configParams["spikeStreamNetworkPassword"] = networkDBInfo.getPassword();
	}
	if(writeArchDB){
		configParams["spikeStreamArchiveHost"] = archiveDBInfo.getHost();
		configParams["spikeStreamArchiveUser"] = archiveDBInfo.getUser();
		configParams["spikeStreamArchivePassword"] = archiveDBInfo.getPassword();
	}
	if(writeAnaDB){
		configParams["spikeStreamAnalysisHost"] = analysisDBInfo.getHost();
		configParams["spikeStreamAnalysisUser"] = analysisDBInfo.getUser();
		configParams["spikeStreamAnalysisPassword"] = analysisDBInfo.getPassword();
	}

	//Set this information in the configuration file
	ConfigEditor configEditor;
	configEditor.setConfigParameters(configParams);
}


/*! Shows a message box to confirm that the user wants to carry out a particular operation */
bool DBConfigMainWindow::showConfirmMessage(const QString& msg){
	QMessageBox msgBox;
	msgBox.setText("Resetting Database");
	msgBox.setInformativeText(msg);
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	int ret = msgBox.exec();
	if(ret == QMessageBox::Ok)
		return true;
	return false;
}



