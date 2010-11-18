//SpikeStream includes
#include "EventRouter.h"
#include "Globals.h"
#include "ConnectionMatrixImporterWidget.h"
#include "ParametersDialog.h"
#include "SpikeStreamException.h"
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
	networkNameEdit = new QLineEdit("Unnamed");
	networkNameEdit->setMinimumSize(250, 30);
	gridLayout->addWidget(networkNameEdit, rowCntr, 1);

	//Button to enable parameters to be set
	QPushButton* parametersButton = new QPushButton("Parameters");
	connect(parametersButton, SIGNAL(clicked()), this, SLOT(setParameters()));
	gridLayout->addWidget(parametersButton, rowCntr, 2);
	++rowCntr;

	//Node Names
	gridLayout->addWidget(new QLabel("Node Names File: "), rowCntr, 0);
	nodeNamesFilePathEdit = new QLineEdit("C:/Users/daogamez/Home/Experiments/HagmanConnectivityMatrix/AnatomicalLabels.dat");
	connect(nodeNamesFilePathEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkImportEnabled(const QString&)));
	nodeNamesFilePathEdit->setMinimumSize(250, 30);
	gridLayout->addWidget(nodeNamesFilePathEdit, rowCntr, 1);
	QPushButton* nodeNamesFileButt = new QPushButton("Browse");
	connect (nodeNamesFileButt, SIGNAL(clicked()), this, SLOT(getNodeNamesFile()));
	gridLayout->addWidget(nodeNamesFileButt, rowCntr, 2);
	++rowCntr;

	//Coordinates
	gridLayout->addWidget(new QLabel("Talairach Coordinates File: "), rowCntr, 0);
	coordinatesFilePathEdit = new QLineEdit("C:/Users/daogamez/Home/Experiments/HagmanConnectivityMatrix/TalairachCoordinates.dat");
	connect(coordinatesFilePathEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkImportEnabled(const QString&)));
	coordinatesFilePathEdit->setMinimumSize(250, 30);
	gridLayout->addWidget(coordinatesFilePathEdit, rowCntr, 1);
	QPushButton* coordinatesFileButt = new QPushButton("Browse");
	connect (coordinatesFileButt, SIGNAL(clicked()), this, SLOT(getCoordinatesFile()));
	gridLayout->addWidget(coordinatesFileButt, rowCntr, 2);
	++rowCntr;

	//Weights
	gridLayout->addWidget(new QLabel("Connectivity Matrix File: "), rowCntr, 0);
	weightsFilePathEdit = new QLineEdit("C:/Users/daogamez/Home/Experiments/HagmanConnectivityMatrix/C.dat");
	connect(weightsFilePathEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkImportEnabled(const QString&)));
	weightsFilePathEdit->setMinimumSize(250, 30);
	gridLayout->addWidget(weightsFilePathEdit, rowCntr, 1);
	QPushButton* weightsFileButt = new QPushButton("Browse");
	connect (weightsFileButt, SIGNAL(clicked()), this, SLOT(getWeightsFile()));
	gridLayout->addWidget(weightsFileButt, rowCntr, 2);
	++rowCntr;

	//Delays
	gridLayout->addWidget(new QLabel("Delays File: "), rowCntr, 0);
	delaysFilePathEdit = new QLineEdit("C:/Users/daogamez/Home/Experiments/HagmanConnectivityMatrix/L.dat");
	connect(delaysFilePathEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkImportEnabled(const QString&)));
	delaysFilePathEdit->setMinimumSize(250, 30);
	gridLayout->addWidget(delaysFilePathEdit, rowCntr, 1);
	QPushButton* delaysFileButt = new QPushButton("Browse");
	connect (delaysFileButt, SIGNAL(clicked()), this, SLOT(getDelaysFile()));
	gridLayout->addWidget(delaysFileButt, rowCntr, 2);
	++rowCntr;

	//Buttons at bottom of dialog
	importButton = new QPushButton("Import");
	importButton->setEnabled(false);
	connect (importButton, SIGNAL(clicked()), this, SLOT(importMatrix()));
	gridLayout->addWidget(importButton, rowCntr, 2);

	//Add layout to dialog
	mainVBox->addLayout(gridLayout);
	mainVBox->addStretch(5);

	//Build appropriate structures for parameter setting
	buildParameters();

	//Set up progress dialog
	progressDialog = new QProgressDialog(this);
	progressDialog->setAutoClose(false);
	progressDialog->setAutoReset(false);
	progressDialog->setModal(true);

	//Set up importer
	matrixImporter = new MatrixImporter();
	connect(matrixImporter, SIGNAL(progress(int,int,QString)), this, SLOT(updateProgress(int,int,QString)), Qt::QueuedConnection);
	connect(matrixImporter, SIGNAL(finished()), this, SLOT(matrixImporterFinished()));
}


/*! Destructor */
ConnectionMatrixImporterWidget::~ConnectionMatrixImporterWidget(){
	delete matrixImporter;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Checks that each of the file paths points to a valid file. */
void ConnectionMatrixImporterWidget::checkImportEnabled(const QString&){
	bool enabled = true;
	if(!QFile::exists(coordinatesFilePathEdit->text()))
		enabled = false;
	if(!QFile::exists(nodeNamesFilePathEdit->text()))
		enabled = false;
	if(!QFile::exists(weightsFilePathEdit->text()))
		enabled = false;
	if(!QFile::exists(delaysFilePathEdit->text()))
		enabled = false;
	importButton->setEnabled(enabled);
}


/*! Launches a file dialog to select file containing the coordinates
	of the neuron groups. */
void ConnectionMatrixImporterWidget::getCoordinatesFile(){
	coordinatesFilePathEdit->setText(getFilePath("*.dat"));
	checkImportEnabled();
}



/*! Launches a file dialog to select file containing the delays
	of the connections between neuron groups. */
void ConnectionMatrixImporterWidget::getDelaysFile(){
	delaysFilePathEdit->setText(getFilePath("*.dat"));
	checkImportEnabled();
}


/*! Launches a file dialog to select file containing the node names
	of the neuron groups. */
void ConnectionMatrixImporterWidget::getNodeNamesFile(){
	nodeNamesFilePathEdit->setText(getFilePath("*.dat"));
	checkImportEnabled();
}


/*! Launches a file dialog to select file containing the weights
	of the connections between neuron groups. */
void ConnectionMatrixImporterWidget::getWeightsFile(){
	weightsFilePathEdit->setText(getFilePath("*.dat"));
	checkImportEnabled();
}


/*! Starts the import of the matrix. */
void ConnectionMatrixImporterWidget::importMatrix(){
	//Check to see if another network is loaded and not saved.
	if(Globals::networkLoaded() && !Globals::getNetwork()->isSaved()){
		qWarning()<<"This plugin unloads the current network and creates a new network in memory.\nIt cannot continue until the current network is saved or discarded.";
		return;
	}

	//Check parameters etc.
	if(networkNameEdit->text().isEmpty())
		networkNameEdit->setText("Unnamed");

	//Start the import
	progressDialog->show();
	matrixImporter->startImport(
			networkNameEdit->text(), networkNameEdit->text(),
			coordinatesFilePathEdit->text(), nodeNamesFilePathEdit->text(),
			weightsFilePathEdit->text(), delaysFilePathEdit->text(), parameterMap);
}


/*! Called when the matrix importer thread finishes */
void ConnectionMatrixImporterWidget::matrixImporterFinished(){
	if(matrixImporter->isError())
		qCritical()<<matrixImporter->getErrorMessage();
	progressDialog->hide();
}


/*! Informs user about progress with import */
void ConnectionMatrixImporterWidget::updateProgress(int stepsCompleted, int totalSteps, QString message){
	if(progressDialog->isVisible()){
		progressDialog->setMaximum(totalSteps);
		progressDialog->setValue(stepsCompleted);
		progressDialog->setLabelText(message);
	}
}


/*! Sets the parameters for the import */
void ConnectionMatrixImporterWidget::setParameters(){
	try{
		ParametersDialog paramDialog(parameterInfoList, defaultParameterMap, parameterMap);
		if(paramDialog.exec() == QDialog::Accepted)
			parameterMap = paramDialog.getParameters();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Builds the current and default parameter map along with the parameter
	info list. */
void ConnectionMatrixImporterWidget::buildParameters(){
	parameterInfoList.append(ParameterInfo("neuron_group_size", "Size of the neuron group at each node.", ParameterInfo::UNSIGNED_INTEGER));
	defaultParameterMap["neuron_group_size"] = 10;
	parameterMap["neuron_group_size"] = defaultParameterMap["neuron_group_size"];

	parameterInfoList.append(ParameterInfo("proportion_excitatory_neurons", "Proportion of excitatory neurons. Varies between 0 and 1.", ParameterInfo::DOUBLE));
	defaultParameterMap["proportion_excitatory_neurons"] = 0.8;
	parameterMap["proportion_excitatory_neurons"] = defaultParameterMap["proportion_excitatory_neurons"];

	parameterInfoList.append(ParameterInfo("rewire_probability", "Probability that connection will be made outside of the group.\n0.0: All connections within each group; 1.0: All connections between groups.", ParameterInfo::DOUBLE));
	defaultParameterMap["rewire_probability"] = 0.1;
	parameterMap["rewire_probability"] = defaultParameterMap["rewire_probability"];

	parameterInfoList.append(ParameterInfo("min_intra_delay", "Minimum delay for a connection within the group.", ParameterInfo::UNSIGNED_INTEGER));
	defaultParameterMap["min_intra_delay"] = 1.0;
	parameterMap["min_intra_delay"] = defaultParameterMap["min_intra_delay"];

	parameterInfoList.append(ParameterInfo("max_intra_delay", "Maximum delay for a connection within the group.", ParameterInfo::UNSIGNED_INTEGER));
	defaultParameterMap["max_intra_delay"] = 1.0;
	parameterMap["max_intra_delay"] = defaultParameterMap["max_intra_delay"];

	parameterInfoList.append(ParameterInfo("min_excitatory_weight", "Minimum weight for an excitatory connection.", ParameterInfo::DOUBLE));
	defaultParameterMap["min_excitatory_weight"] = 0.0;
	parameterMap["min_excitatory_weight"] = defaultParameterMap["min_excitatory_weight"];

	parameterInfoList.append(ParameterInfo("max_excitatory_weight", "Maximim weight for an excitatory connection.", ParameterInfo::DOUBLE));
	defaultParameterMap["max_excitatory_weight"] = 0.5;
	parameterMap["max_excitatory_weight"] = defaultParameterMap["max_excitatory_weight"];

	parameterInfoList.append(ParameterInfo("min_inhibitory_weight", "Minimum weight for an inhibitory connection.", ParameterInfo::DOUBLE));
	defaultParameterMap["min_inhibitory_weight"] = -1.0;
	parameterMap["min_inhibitory_weight"] = defaultParameterMap["min_inhibitory_weight"];

	parameterInfoList.append(ParameterInfo("max_inhibitory_weight", "Maximim weight for an inhibitory connection.", ParameterInfo::DOUBLE));
	defaultParameterMap["max_inhibitory_weight"] = 0.0;
	parameterMap["max_inhibitory_weight"] = defaultParameterMap["max_inhibitory_weight"];
}


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




