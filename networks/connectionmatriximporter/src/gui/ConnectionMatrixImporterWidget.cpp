//SpikeStream includes
#include "EventRouter.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "ConnectionMatrixImporterWidget.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QFileDialog>

//Other includes
#include <typeinfo>

//Functions for dynamic library loading
extern "C" {
	/*! Creates a ConnectionMatrixImporterWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new ConnectionMatrixImporterWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Connection Matrix Importer");
	}
}


/*! Constructor */
ConnectionMatrixImporterWidget::ConnectionMatrixImporterWidget(){
	int rowCntr = 0;

	//Create layouts to organise dialog
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout();

	//Field to enable user to enter network name
	gridLayout->addWidget(new QLabel("Network name: "), rowCntr, 0);
	networkName = new QLineEdit("Unnamed");
	networkName->setMinimumSize(250, 30);
	gridLayout->addWidget(networkName, rowCntr, 1);
	++rowCntr;

	//Coordinates
	gridLayout->addWidget(new QLabel("Talairach Coordinates File: "), rowCntr, 0);
	coordinatesFilePath = new QLineEdit();
	connect(coordinatesFilePath, SIGNAL(textChanged(const QString&)), this, SLOT(checkImportEnabled(const QString&)));
	coordinatesFilePath->setMinimumSize(250, 30);
	gridLayout->addWidget(coordinatesFilePath, rowCntr, 1);
	QPushButton* coordinatesFileButt = new QPushButton("Browse");
	connect (coordinatesFileButt, SIGNAL(clicked()), this, SLOT(getCoordinatesFile()));
	gridLayout->addWidget(coordinatesFileButt, rowCntr, 2);
	++rowCntr;

	//Buttons at bottom of dialog
	importButton = new QPushButton("Import");
	importButton->setEnabled(false);
	connect (importButton, SIGNAL(clicked()), this, SLOT(importMatrix()));
	gridLayout->addWidget(importButton, rowCntr, 2);

	//Add layout to dialog
	mainVBox->addLayout(gridLayout);
	mainVBox->addStretch(5);

}


/*! Destructor */
ConnectionMatrixImporterWidget::~ConnectionMatrixImporterWidget(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Checks that each of the file paths points to a valid file. */
void ConnectionMatrixImporterWidget::checkImportEnabled(const QString&){
	bool enabled = false;
	if(QFile::exists(coordinatesFilePath->text()))
		enabled = true;
	importButton->setEnabled(enabled);
}


/*! Launches a file dialog to select file containing the coordinates
	of the neuron groups. */
void ConnectionMatrixImporterWidget::getCoordinatesFile(){
	coordinatesFilePath->setText(getFilePath("*.dat"));
	checkImportEnabled();
}


/*! Starts the import of the matrix. */
void ConnectionMatrixImporterWidget::importMatrix(){

}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Enables user to enter a file path */
QString ConnectionMatrixImporterWidget::getFilePath(QString fileFilter){
	QFileDialog dialog(this);
	dialog.setDirectory(Globals::getWorkingDirectory());
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter( QString("Configuration files (" + fileFilter + ")") );
	dialog.setViewMode(QFileDialog::Detail);
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	if(fileNames.size() > 0)
		return fileNames[0];
	else
		return QString("");
}




