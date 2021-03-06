//SpikeStream includes
#include "NRMConstants.h"
#include "NRMImportWidget.h"
#include "Globals.h"
#include "NRMException.h"
#include "NumberConversionException.h"
#include "Util.h"
#include "Box.h"
#include "NetworkDao.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QProgressBar>
#include <QMessageBox>

//Other includes
#include <iostream>
using namespace std;


/*! The type ID of a weightless neuron in the NeuronTypes table of the SpikeStreamNetwork database */
#define WEIGHTLESS_NEURON_TYPE_ID 3



//Functions for dynamic library loading
extern "C" {
	/*! Creates a NRMImporterWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new NRMImportWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("NRM Importer");
	}
}

/*! Constructor */
NRMImportWidget::NRMImportWidget() {
    //Build the fixed pages of the widget and make sure page2 is null
    buildPage1();
    buildPage2();
    buildBusyPage();
    buildSuccessPage();

    //Create the nrm loader, which will run as a separate thread
    fileLoader = new NRMFileLoader();
    connect (fileLoader, SIGNAL(finished()), this, SLOT(threadFinished()));
    dataImporter = new NRMDataImporter(Globals::getNetworkDao()->getDBInfo(), Globals::getArchiveDao()->getDBInfo());
    connect(dataImporter, SIGNAL(finished()), this, SLOT(threadFinished()));

    //Connect this class up to the event router to inform other classes when the network list changes
    connect(this, SIGNAL(networkListChanged()), Globals::getEventRouter(), SLOT(networkListChangedSlot()));

    //Set initial state of variables
    currentTask = -1;
    operationCancelled = false;

    //Show the first page
    showPage1();
}


/*! Destructor */
NRMImportWidget::~NRMImportWidget(){
    delete fileLoader;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Builds the network based on the information stored in the dialog and entered by the user */
void NRMImportWidget::addNetwork(){
    operationCancelled = false;
    currentTask = -1;

    //Make sure network name and description have text
    if(networkName->text() == "")
		networkName->setText("Unnamed");
    if(networkDescription->text() == "")
		networkDescription->setText("Undescribed");

    //Lists of the input and neural layers in the NRM network
    QList<NRMInputLayer*> inputList = fileLoader->getNetwork()->getAllInputs();
    QList<NRMNeuralLayer*> neuralList = fileLoader->getNetwork()->getAllNeuralLayers();

    //Clear list holding neuron groups to be added to the network
    newNeuronGroupList.clear();

    /* Work through input layers in the same way as was done when adding them to the
	page in addLayersToPage2() method */
    int rowCount = 0;
    for(int i=0; i<inputList.size(); ++i){
		int neurGrpXPos, neurGrpYPos, neurGrpZPos;

		//Extract position information
		try{
			QWidget* lineEdit = layerLocationGrid->itemAtPosition(rowCount, page2XCol)->widget();
			neurGrpXPos = Util::getInt(((QLineEdit*)lineEdit)->text());
			lineEdit = layerLocationGrid->itemAtPosition(rowCount, page2YCol)->widget();
			neurGrpYPos = Util::getInt(((QLineEdit*)lineEdit)->text());
			lineEdit = layerLocationGrid->itemAtPosition(rowCount, page2ZCol)->widget();
			neurGrpZPos = Util::getInt(((QLineEdit*)lineEdit)->text());
		}
		catch (NumberConversionException& ex){
			qCritical()<<"Error in neuron group position: "<<ex.getMessage();
			return;
		}

		//Create a neuron info describing group and use it to create a new group
		NeuronGroupInfo tmpGrpInfo(0, inputList[i]->frameName.data(), inputList[i]->frameName.data(), QHash<QString, double>(), Globals::getNetworkDao()->getNeuronType(WEIGHTLESS_NEURON_TYPE_ID));
		NeuronGroup* tmpNeurGrp = new NeuronGroup(tmpGrpInfo);

		//Add layer with width, height and position to group
		tmpNeurGrp->addLayer(inputList[i]->width, inputList[i]->height, neurGrpXPos, neurGrpYPos, neurGrpZPos);
		newNeuronGroupList.append(tmpNeurGrp);

		++rowCount;
    }

    /* Work through neural layers in the same way as was done when adding them to the
	page in addLayersToPage2() method */
    for(int i=0; i< neuralList.size(); ++i){
		int neurGrpXPos, neurGrpYPos, neurGrpZPos;

		//Extract position information
		try{
			QWidget* lineEdit = layerLocationGrid->itemAtPosition(rowCount, page2XCol)->widget();
			neurGrpXPos = Util::getInt(((QLineEdit*)lineEdit)->text());
			lineEdit = layerLocationGrid->itemAtPosition(rowCount, page2YCol)->widget();
			neurGrpYPos = Util::getInt(((QLineEdit*)lineEdit)->text());
			lineEdit = layerLocationGrid->itemAtPosition(rowCount, page2ZCol)->widget();
			neurGrpZPos = Util::getInt(((QLineEdit*)lineEdit)->text());
		}
		catch (NumberConversionException& ex){
			qCritical()<<"Error in neuron group position: "<<ex.getMessage();
			return;
		}

		//Create a neuron info describing group and use it to create a new group
		NeuronGroupInfo tmpGrpInfo(0, neuralList[i]->frameName.data(), neuralList[i]->frameName.data(), QHash<QString, double>(), Globals::getNetworkDao()->getNeuronType(WEIGHTLESS_NEURON_TYPE_ID));
		NeuronGroup* tmpNeurGrp = new NeuronGroup(tmpGrpInfo);

		//Add layer with width, height and position to group
		tmpNeurGrp->addLayer(neuralList[i]->width, neuralList[i]->height, neurGrpXPos, neurGrpYPos, neurGrpZPos);
		newNeuronGroupList.append(tmpNeurGrp);

		++rowCount;
    }

    //Add neuron groups and connection groups to database
    newNetwork = NULL;
    try{
		//FIXME: THIS IS MESSY: MOVE INTO DATA IMPORTER

		//Create network
		newNetwork = new Network(networkName->text(), networkDescription->text(), Globals::getNetworkDao()->getDBInfo(), Globals::getArchiveDao()->getDBInfo());
		connect(newNetwork, SIGNAL(taskFinished()), this, SLOT(threadFinished()));
		showBusyPage("Adding neurons to database...");

		/* Add all of the neuron groups to the network
			This network starts a thread. Need to show busy dialog and periodically check to see if thread has
			finished using a timer. */
		newNetwork->addNeuronGroups(newNeuronGroupList);

		//Show busy page and wait for thread to finish
		currentTask = ADD_NEURON_GROUPS_TASK;
    }
    catch (SpikeStreamException& ex){
		qCritical()<<"Error occurred adding neuron groups to network: "<<ex.getMessage();

		//Delete network from database
		if(newNetwork != NULL){
			NetworkDaoThread netDaoThread(Globals::getNetworkDao()->getDBInfo());
			netDaoThread.startDeleteNetwork(newNetwork->getID());
			netDaoThread.wait();
		}
    }
}


/*! Cancels the operation that is currently in progress. */
void NRMImportWidget::cancel(){
    operationCancelled = true;
    switch (currentTask){
		case FILE_LOADING_TASK:
			fileLoader->stop();
			break;
		case ADD_NEURON_GROUPS_TASK:
			newNetwork->cancel();
			break;
		case ADD_CONNECTION_GROUPS_TASK:
			dataImporter->stop();
			break;
		default:
			qCritical()<<"Current task with id " + QString::number(currentTask) + " is not recognized.";
    }
}


/*! Called when a thread doing potentially heavy operations finishes */
void NRMImportWidget::threadFinished(){
    //Operation has been cancelled by user interaction with buttons in this widget
    if(operationCancelled){
		switch (currentTask){
			case FILE_LOADING_TASK:
				showPage1();
			break;
			case ADD_NEURON_GROUPS_TASK: case ADD_CONNECTION_GROUPS_TASK: case ADD_TRAINING_TASK:
				showPage2();
			break;
			default:
				qCritical()<<"Current task with id " + QString::number(currentTask) + " is not recognized.";
		}
		currentTask = -1;
		operationCancelled = false;
		return;
    }

    //Check for errors
    bool threadError = false;
    switch (currentTask){
		case FILE_LOADING_TASK:
			if(fileLoader->isError()){
				qCritical()<<fileLoader->getErrorMessage();
				threadError = true;
			}
		break;
		case ADD_NEURON_GROUPS_TASK:
			if(newNetwork->isError()){
				qCritical()<<newNetwork->getErrorMessage();
				NetworkDaoThread netDaoThread(Globals::getNetworkDao()->getDBInfo());
				netDaoThread.startDeleteNetwork(newNetwork->getID());
				netDaoThread.wait();
				threadError = true;
			}
		break;
		case ADD_CONNECTION_GROUPS_TASK: case ADD_ARCHIVES_TASK: case ADD_TRAINING_TASK:
			if(dataImporter->isError()){
				qCritical()<<dataImporter->getErrorMessage();
				threadError = true;
			}
		break;
		default:
			qCritical()<<"Current task with id " + QString::number(currentTask) + " is not recognized.";
    }
    if(threadError){
		showPage1();
		currentTask = -1;
		return;
    }

    /* Operation has not been cancelled and thread has terminated normally and without errors
		Carry out appropriate operation depending on the current task */
    switch (currentTask){
		case FILE_LOADING_TASK:
			addLayersToPage2();
			showPage2();
		break;
		case ADD_NEURON_GROUPS_TASK:
			//Store the new neuron group IDs in the NRM network
			addNeuronGroupIDsToNRMNetwork();

			//Disconnect network - FIXME ADD NEURONS SHOULD BE MOVED INTO DATA IMPORTER
			disconnect(newNetwork, SIGNAL(taskFinished()), this, SLOT(threadFinished()));

			//Start the add connection groups to network task
			addConnectionGroups();
		break;
		case ADD_CONNECTION_GROUPS_TASK:
			//Add the archives to the database
			addArchives();
		break;
		case ADD_ARCHIVES_TASK:
			//Add training to the database
			addTraining();
		break;
		case ADD_TRAINING_TASK:
			/* Inform other classes that the list of networks has changed
			   No need to inform about archive changes because the new network is not loaded. */
			emit networkListChanged();

			//Inform user that it went ok
			showSuccessPage();

			//Clean up network from memory - this does not affect the network stored in the database
			if(newNetwork != NULL)
				delete newNetwork;
		break;
		default:
			qCritical()<<"Current task with id " + QString::number(currentTask) + " is not recognized.";
    }

}


/*! When user clicks on 'browse' file dialog is displayed to select the config file */
void NRMImportWidget::getConfigFile(){
    QString filePath = getFilePath("*.cfg");
    configFilePath->setText(filePath);
}


/*! When user clicks on 'browse' file dialog is displayed to select the dataset file */
void NRMImportWidget::getDatasetFile(){
    QString filePath = getFilePath("*.set");
    datasetFilePath->setText(filePath);
}


/*! When user clicks on 'browse' file dialog is displayed to select the training file */
void NRMImportWidget::getTrainingFile(){
    QString filePath = getFilePath("*.ntr");
    trainingFilePath->setText(filePath);
}


/*! Displays the busy page */
void NRMImportWidget::showBusyPage(QString busyMessage){
    busyLabel->setText(busyMessage);
    busyWidget->setVisible(true);
    page1Widget->setVisible(false);
    page2Widget->setVisible(false);
    successWidget->setVisible(false);
    adjustSize();
}


/*! Shows the first page of the wizard */
void NRMImportWidget::showPage1(){
    configFilePath->setStyleSheet("* { color: black; }");
    configFilePath->setSelection(0, 0);
    datasetFilePath->setStyleSheet("* { color: black; }");
    datasetFilePath->setSelection(0, 0);
    trainingFilePath->setStyleSheet("* { color: black; }");
    trainingFilePath->setSelection(0, 0);
    busyWidget->setVisible(false);
    page1Widget->setVisible(true);
    page2Widget->setVisible(false);
    successWidget->setVisible(false);
    adjustSize();
}


/*! Shows the second page of the wizard */
void NRMImportWidget::showPage2(){
    busyWidget->setVisible(false);
    page1Widget->setVisible(false);
    page2Widget->setVisible(true);
    successWidget->setVisible(false);
    adjustSize();
}


/*! Shows final page when import has been completed */
void NRMImportWidget::showSuccessPage(){
    busyWidget->setVisible(false);
    page1Widget->setVisible(false);
    page2Widget->setVisible(false);
    successWidget->setVisible(true);
}


/*! Loads up config and information from files, which is used to construct page 2. */
void NRMImportWidget::loadNetworkFromFiles(){
    bool fileError = false;
    operationCancelled = false;
    currentTask = -1;

    //Check that file paths are valid
    if(!QFile::exists(configFilePath->text())){
		if(configFilePath->text() == "")
			configFilePath->setText("Please enter a valid file path");
		configFilePath->setStyleSheet("* { color: red; }");
		configFilePath->setSelection(0, 0);
		fileError = true;
    }
    else{
		configFilePath->setStyleSheet("* { color: black; }");
    }
    if(!QFile::exists(datasetFilePath->text())){
		if(datasetFilePath->text() == "")
			datasetFilePath->setText("Please enter a valid file path");
		datasetFilePath->setStyleSheet("* { color: red; }");
		datasetFilePath->setSelection(0, 0);
		fileError = true;
    }
    else{
		datasetFilePath->setStyleSheet("* { color: black; }");
    }
    if(!QFile::exists(trainingFilePath->text())){
		if(trainingFilePath->text() == "")
			trainingFilePath->setText("Please enter a valid file path");
		trainingFilePath->setStyleSheet("* { color: red; }");
		trainingFilePath->setSelection(0, 0);
		fileError = true;
    }
    else{
		trainingFilePath->setStyleSheet("* { color: black; }");
    }
    if(fileError)
		return;

    //Load up configuration file
    showBusyPage(QString ("Loading configuration and training files"));
    try{
		currentTask = FILE_LOADING_TASK;
		fileLoader->setConfigFilePath(configFilePath->text());
		fileLoader->setDatasetFilePath(datasetFilePath->text());
		fileLoader->setTrainingFilePath(trainingFilePath->text());
		fileLoader->start();
    }
    catch(NRMException& ex){
		qCritical()<<ex.getMessage();
		showPage1();
    }
    catch(...){
		qCritical()<<"An unknown exception occurred whilst loading NRM files";
		showPage1();
    }
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Stores data set containing firing pattern as an archive. */
void NRMImportWidget::addArchives(){
    //Do nothing if there is no data
	if(fileLoader->getDataSet()->size() == 0){
		qDebug()<<"No archive data.";
		return;
	}

    //Reset variables
    operationCancelled = false;
    currentTask = ADD_ARCHIVES_TASK;
    showBusyPage("Adding archives to database...");
    try{
		if(dataImporter->isRunning())
			throw SpikeStreamException("Trying to add archives, but data importer is already running.");
		dataImporter->prepareAddArchives(fileLoader->getNetwork(), newNetwork, fileLoader->getDataSet());
		dataImporter->start();
    }
    catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		showPage1();
		currentTask = -1;
    }
}


/*! Adds connection groups and connections to the new network */
void NRMImportWidget::addConnectionGroups(){
    //Reset variables
    operationCancelled = false;
    currentTask = ADD_CONNECTION_GROUPS_TASK;
    showBusyPage("Adding connections to database...");
    dataImporter->prepareAddConnections(fileLoader->getNetwork(), newNetwork);
    dataImporter->start();
}


/*! Adds the network layers to page 2 so that user can fine tune their
    final locations in SpikeStream */
void NRMImportWidget::addLayersToPage2(){
    //Check network has been loaded
    if(fileLoader->getNetwork() == NULL){
		qCritical()<<"Network has not been loaded. Cannot add network layers to dialog.";
		return;
    }

    //Clear grid layout
    for(int x=0; x<layerLocationGrid->rowCount(); ++x){
		for(int y=0; y<layerLocationGrid->columnCount(); ++y){
			layerLocationGrid->removeItem(layerLocationGrid->itemAtPosition(x, y));
		}
    }

    //Get the input and neural layers
    QList<NRMInputLayer*> inputList = fileLoader->getNetwork()->getAllInputs();
    QList<NRMNeuralLayer*> neuralList = fileLoader->getNetwork()->getAllNeuralLayers();

    //Variables for addition of layers to dialog
    int rowCount = 0, tmpXPos = 1, tmpYPos = 1, tmpZPos = 1;

    //Add input layers to grid layout.
    for(int i=0; i<inputList.size(); ++i){
		//Add widgets to grid
		layerLocationGrid->addWidget(new QLabel( QString::number(inputList[i]->frameNum) ), rowCount, 0);
		layerLocationGrid->addWidget(new QLabel( inputList[i]->frameName.data() ), rowCount, 1);
		QString sizeStr = "(" + QString::number(inputList[i]->width) + "x" + QString::number(inputList[i]->height) + "):";
		layerLocationGrid->addWidget(new QLabel(sizeStr), rowCount, 2);//size
		layerLocationGrid->addWidget(new QLabel(" x="), rowCount, 3);//size
		layerLocationGrid->addWidget(new QLineEdit( QString::number(tmpXPos) ), rowCount, page2XCol);//x location
		layerLocationGrid->addWidget(new QLabel(" y="), rowCount, 5);//size
		layerLocationGrid->addWidget(new QLineEdit( QString::number(tmpYPos) ), rowCount, page2YCol);//y location
		layerLocationGrid->addWidget(new QLabel(" z="), rowCount, 7);//size
		layerLocationGrid->addWidget(new QLineEdit( QString::number(tmpZPos) ), rowCount, page2ZCol);//z location
		++rowCount;

		//Calculate next proposed insertion position
		tmpXPos += inputList[i]->width + 2;
    }

    //Add neural layers to grid layout.
    for(int i=0; i<neuralList.size(); ++i){
		//Add widgets to grid
		layerLocationGrid->addWidget(new QLabel( QString::number(neuralList[i]->frameNum) ), rowCount, 0);
		layerLocationGrid->addWidget(new QLabel( neuralList[i]->frameName.data() ), rowCount, 1);
		QString sizeStr = "(" + QString::number(neuralList[i]->width) + "x" + QString::number(neuralList[i]->height) + "):";
		layerLocationGrid->addWidget(new QLabel(sizeStr), rowCount, 2);//size
		layerLocationGrid->addWidget(new QLabel(" x="), rowCount, 3);//size
		layerLocationGrid->addWidget(new QLineEdit( QString::number(tmpXPos) ), rowCount, 4);//x location
		layerLocationGrid->addWidget(new QLabel(" y="), rowCount, 5);//size
		layerLocationGrid->addWidget(new QLineEdit( QString::number(tmpYPos) ), rowCount, 6);//y location
		layerLocationGrid->addWidget(new QLabel(" z="), rowCount, 7);//size
		layerLocationGrid->addWidget(new QLineEdit( QString::number(tmpZPos) ), rowCount, 8);//z location
		++rowCount;

		//Calculate next proposed insertion position
		tmpXPos += neuralList[i]->width + 2;
    }
}


/*! Stores the SpikeStream neuron group ID inside the NRM layers to enable
    the connections to be added correctly. */
void NRMImportWidget::addNeuronGroupIDsToNRMNetwork(){
    //Lists of the input and neural layers in the NRM network
    QList<NRMInputLayer*> inputList = fileLoader->getNetwork()->getAllInputs();
    QList<NRMNeuralLayer*> neuralList = fileLoader->getNetwork()->getAllNeuralLayers();

    //Check that the lists are likely to be in synchrony
    if( (inputList.size() + neuralList.size()) != newNeuronGroupList.size()){
		qCritical()<<"NRM layers and new neuron groups do not match.";
		return;
    }

    //Work through list in same order as was used to generate the list
    int index = 0;
    for(int i=0; i<inputList.size(); ++i){
		inputList[i]->spikeStreamID = newNeuronGroupList[index]->getID();
		++index;
    }
    for(int i=0; i<neuralList.size(); ++i){
		neuralList[i]->spikeStreamID = newNeuronGroupList[index]->getID();
		++index;
    }
}


/*! Prepares the data importer to add training to the database */
void NRMImportWidget::addTraining(){
    operationCancelled = false;
    currentTask = ADD_TRAINING_TASK;
    showBusyPage("Adding training to database...");
    try{
		dataImporter->prepareAddTraining(fileLoader->getNetwork(), newNetwork);
		dataImporter->start();
    }
    catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		showPage1();
		currentTask = -1;
    }
}


/*! Builds page to display when an operation is taking place */
void NRMImportWidget::buildBusyPage(){
    busyWidget = new QWidget(this);
	QVBoxLayout *verticalBox = new QVBoxLayout(busyWidget);
    verticalBox->setMargin(10);

    QHBoxLayout *hBox = new QHBoxLayout();
    hBox->addStretch(5);
    busyLabel = new QLabel("Operation in progress");
    hBox->addWidget(busyLabel);

    //Progress bar
    QProgressBar* progBar = new QProgressBar();
    progBar->setRange(0, 0);
    hBox->addSpacing(10);
    hBox->addWidget(progBar);

    //Cancel button
    QPushButton* cancelButton = new QPushButton("Cancel");
    connect (cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    hBox->addWidget(cancelButton);
    hBox->addStretch(10);
    verticalBox->addSpacing(10);
    verticalBox->addLayout(hBox);
}


/*! Builds the first page of the widget */
void NRMImportWidget::buildPage1(){
    page1Widget = new QWidget(this);

    //Create box to organise dialog
	QVBoxLayout *verticalBox = new QVBoxLayout(page1Widget);
    verticalBox->setMargin(10);

    //Layout path gathering information using grid layout
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setMargin(10);

    //Configuration information
    gridLayout->addWidget(new QLabel("NRM Configuration File: "), 0, 0);
    configFilePath = new QLineEdit();
    configFilePath->setMinimumSize(250, 30);
    gridLayout->addWidget(configFilePath, 0, 1);
    QPushButton* configFileButt = new QPushButton("Browse");
    connect (configFileButt, SIGNAL(clicked()), this, SLOT(getConfigFile()));
    gridLayout->addWidget(configFileButt, 0, 2);

    //Training information
    gridLayout->addWidget(new QLabel("NRM Training File: "), 1, 0);
    trainingFilePath = new QLineEdit();
    trainingFilePath->setMinimumSize(250, 30);
    gridLayout->addWidget(trainingFilePath, 1, 1);
    QPushButton* trainingFileButt = new QPushButton("Browse");
    connect (trainingFileButt, SIGNAL(clicked()), this, SLOT(getTrainingFile()));
    gridLayout->addWidget(trainingFileButt, 1, 2);

    //Data set information
    gridLayout->addWidget(new QLabel("NRM Dataset File: "), 2, 0);
    datasetFilePath = new QLineEdit();
    datasetFilePath->setMinimumSize(250, 30);
    gridLayout->addWidget(datasetFilePath, 2, 1);
    QPushButton* datasetFileButt = new QPushButton("Browse");
    connect (datasetFileButt, SIGNAL(clicked()), this, SLOT(getDatasetFile()));
    gridLayout->addWidget(datasetFileButt, 2, 2);

    //Add layout to dialog
    verticalBox->addLayout(gridLayout);

    //Buttons at bottom of dialog
    QHBoxLayout *buttonBox = new QHBoxLayout();
    QPushButton* nextButton = new QPushButton("Next");
	nextButton->setMinimumSize(80, 30);
	buttonBox->addStretch(5);
    buttonBox->addWidget(nextButton);
    connect (nextButton, SIGNAL(clicked()), this, SLOT(loadNetworkFromFiles()));
    verticalBox->addLayout(buttonBox);
}


/*! Builds the second page of the widget */
void NRMImportWidget::buildPage2(){
    //Build new page 2 widget
    page2Widget = new QWidget(this);

    //Create box to organise dialog
	QVBoxLayout *verticalBox = new QVBoxLayout(page2Widget);
    verticalBox->setMargin(20);

    layerLocationGrid = new QGridLayout();
    layerLocationGrid->setHorizontalSpacing(5);
    layerLocationGrid->setColumnMinimumWidth(page2XCol, 50);//X position
    layerLocationGrid->setColumnMinimumWidth(page2YCol, 50);//Y position
    layerLocationGrid->setColumnMinimumWidth(page2ZCol, 50);//Z position
    verticalBox->addLayout(layerLocationGrid);
    verticalBox->addSpacing(10);

    //Name
    QHBoxLayout *nameBox = new QHBoxLayout();
    nameBox->addWidget(new QLabel("Network Name: "));
    networkName = new QLineEdit("Unnamed");
    networkName->setMinimumSize(250, 30);
    nameBox->addWidget(networkName);
    nameBox->addStretch(5);
    verticalBox->addLayout(nameBox);

    //Description
    QHBoxLayout *descBox = new QHBoxLayout();
    descBox->addWidget(new QLabel("Network Description: "));
    networkDescription = new QLineEdit("Undescribed");
    networkDescription->setMinimumSize(250, 30);
    descBox->addWidget(networkDescription);
    descBox->addStretch(5);
    verticalBox->addLayout(descBox);

    //Buttons at bottom of dialog
    QHBoxLayout *buttonBox = new QHBoxLayout();
    QPushButton* backButton = new QPushButton("Back");
    buttonBox->addWidget(backButton);
    connect (backButton, SIGNAL(clicked()), this, SLOT(showPage1()));
    QPushButton* finishButton = new QPushButton("Finish");
    buttonBox->addWidget(finishButton);
    connect (finishButton, SIGNAL(clicked()), this, SLOT(addNetwork()));
    verticalBox->addLayout(buttonBox);
}


/*! Constructs the final page informing user that loading is complete */
void NRMImportWidget::buildSuccessPage(){
    successWidget = new QWidget(this);
	QVBoxLayout *verticalBox = new QVBoxLayout(successWidget);
    verticalBox->setMargin(10);
    verticalBox->addWidget(new QLabel("NRM loading complete"));
}


/*! Enables user to enter a file path */
QString NRMImportWidget::getFilePath(QString fileFilter){
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


