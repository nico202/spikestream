#ifndef NRMIMPORTDIALOG_H
#define NRMIMPORTDIALOG_H

//SpikeStream includes
#include "NRMDataImporter.h"
#include "NRMFileLoader.h"
#include "NRMNetwork.h"

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

    private slots:
	void addNetwork();
	void cancel();
	void getConfigFile();
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

	/*! Set to true when file loading is taking place in a separate thread */
	bool fileLoading;

	/*! Set to true when data is being imported in a separate thread */
	bool dataImporting;

	/*! Runs as a separate thread to load data from configuration and training files */
	NRMFileLoader* fileLoader;

	/*! Runs as a separate thread to load a NRM network into the database */
	NRMDataImporter* dataImporter;


	//=======================  METHODS  =========================
	void addLayersToPage2();
	void buildBusyPage();
	void buildPage1();
	void buildPage2();
	void buildSuccessPage();
	QString getFilePath(QString fileFilter);

};

#endif//NRMIMPORTDIALOG_H


