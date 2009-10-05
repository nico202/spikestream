#ifndef NRMIMPORTDIALOG_H
#define NRMIMPORTDIALOG_H

//SpikeStream includes
#include "NRMDataImporter.h"
#include "NRMFileLoader.h"
#include "NRMNetwork.h"
#include "Network.h"
using namespace spikestream;

//Qt includes
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QWidget>

class NRMImportDialog : public QDialog {
    Q_OBJECT

    public:
	NRMImportDialog(QWidget* parent);
	~NRMImportDialog();

    signals:
	void networkListChanged();

    private slots:
	void addNetwork();
	void cancel();
	void getConfigFile();
	void getDatasetFile();
	void getTrainingFile();
	void loadNetworkFromFiles();
	void showBusyPage(QString msg);
	void showPage1();
	void showPage2();
	void showSuccessPage();
	void threadFinished();

    private:
	//======================  VARIABLES  =======================
	/*! Allows user to enter the path to the configuration file */
	QLineEdit* configFilePath;

	/*! Allows user to enter the path to the training file */
	QLineEdit* trainingFilePath;

	/*! Allows user to enter the path to the dataset file */
	QLineEdit* datasetFilePath;

	/*! Displayed when thread-based operations are taking place, such as
	    file loading and data importing */
	QWidget* busyWidget;

	/*! Label explaning which operation is in progress */
	QLabel* busyLabel;

	/*! Grid layout for page2 holding components that enable
	    user to edit location of layers */
	QGridLayout* layerLocationGrid;

	/*! Holds all of the controls for the first stage of the dialog */
	QWidget* page1Widget;

	/*! Holds all of the controls for the second stage of the dialog */
	QWidget* page2Widget;

	/*! Displayed when data has been successfully imported */
	QWidget* successWidget;

	/*! Records if an operation has been cancelled by the user */
	bool operationCancelled;

	/*! The current task the dialog is performing. Different task types are given below */
	int currentTask;

	/*! Task of loading file in a separate thread */
	static const int FILE_LOADING_TASK = 1;

	/*! Task of adding a network to the database */
	static const int ADD_NEURON_GROUPS_TASK = 2;

	/*! Task of adding a network to the database */
	static const int ADD_CONNECTION_GROUPS_TASK = 3;

	/*! Task of converting data sets to archives */
	static const int ADD_ARCHIVES_TASK = 4;

	/*! Task of adding training to the database */
	static const int ADD_TRAINING_TASK = 5;

	/*! Runs as a separate thread to load data from configuration and training files */
	NRMFileLoader* fileLoader;

	/*! Runs as a separate thread to load connections */
	NRMDataImporter* dataImporter;

	/*! Network that is being imported */
	Network* newNetwork;

	/*! List of neuron groups that are added to the network */
	QList<NeuronGroup*> newNeuronGroupList;

	/*! Name assigned to the network being imported */
	QLineEdit* networkName;

	/*! Description of the network being imported */
	QLineEdit* networkDescription;

	/*! Location of the x QLineEdit widget */
	static const int page2XCol = 4;

	/*! Location of the y QLineEdit widget */
	static const int page2YCol = 6;

	/*! Location of the z QLineEdit widget */
	static const int page2ZCol = 8;


	//=======================  METHODS  =========================
	void addArchives();
	void addConnectionGroups();
	void addLayersToPage2();
	void addNeuronGroupIDsToNRMNetwork();
	void addTraining();
	void buildBusyPage();
	void buildPage1();
	void buildPage2();
	void buildSuccessPage();
	QString getFilePath(QString fileFilter);
	int getInt();
	void showMessage();

};

#endif//NRMIMPORTDIALOG_H


