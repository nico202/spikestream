//SpikeStream includes
#include "NRMImportDialog.h"
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

/*! Constructor */
NRMImportDialog::NRMImportDialog(QWidget* parent) : QDialog(parent, "NoiseParamDlg", false){

    //Set caption
    this->setCaption("Import NRM Network");

    //Build the fixed pages of the widget and make sure page2 is null
    buildPage1();
    buildPage2();
    buildBusyPage();
    buildSuccessPage();

    //Create the nrm loader, which will run as a separate thread
    fileLoader = new NRMFileLoader();
    connect (fileLoader, SIGNAL(finished()), this, SLOT(threadFinished()));

    //Connect this class up to the event router to inform other classes when the network list changes
    connect(this, SIGNAL(networkListChanged()), Globals::getEventRouter(), SLOT(networkListChangedSlot()));

    //Set initial state of variables
    currentTask = -1;
    operationCancelled = false;

    //Show the first page
    showPage1();

}


/*! Destructor */
NRMImportDialog::~NRMImportDialog(){
    delete fileLoader;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Builds the network based on the information stored in the dialog and entered by the user */
void NRMImportDialog::addNetwork(){
    operationCancelled = false;

    //Make sure network name and description have text
    if(networkName->text() == "")
	networkName->setText("Unnamed");
    if(networkDescription->text() == "")
	networkDescription->setText("Undescribed");

    //Lists of the input and neural layers in the NRM network
    QList<NRMInputLayer*> inputList = fileLoader->getNetwork()->getAllInputs();
    QList<NRMNeuralLayer*> neuralList = fileLoader->getNetwork()->getAllNeuralLayers();

    //List holding neuron groups to be added to the network
    QList<NeuronGroup*> newNeurGrpList;

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
	NeuronGroupInfo tmpGrpInfo(0, inputList[i]->frameName.data(), inputList[i]->frameName.data(), QHash<QString, double>(), 2);
	NeuronGroup* tmpNeurGrp = new NeuronGroup(tmpGrpInfo);

	//Add layer with width, height and position to group
	tmpNeurGrp->addLayer(inputList[i]->width, inputList[i]->height, neurGrpXPos, neurGrpYPos, neurGrpZPos);
	newNeurGrpList.append(tmpNeurGrp);

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
	NeuronGroupInfo tmpGrpInfo(0, neuralList[i]->frameName.data(), neuralList[i]->frameName.data(), QHash<QString, double>(), 2);
	NeuronGroup* tmpNeurGrp = new NeuronGroup(tmpGrpInfo);

	//Add layer with width, height and position to group
	tmpNeurGrp->addLayer(neuralList[i]->width, neuralList[i]->height, neurGrpXPos, neurGrpYPos, neurGrpZPos);
	newNeurGrpList.append(tmpNeurGrp);

	++rowCount;
    }

    //Add neuron groups and connection groups to database
    newNetwork = NULL;
    try{
	//Create network
	newNetwork = new Network(Globals::getNetworkDao(), networkName->text(), networkDescription->text());
	connect(newNetwork, SIGNAL(taskFinished()), this, SLOT(threadFinished()));
	showBusyPage("Adding network to database...");

	/* Add all of the neuron groups to the network
	   This network starts a thread. Need to show busy dialog and periodically check to see if thread has
	   finished using a timer. */
	newNetwork->addNeuronGroups(newNeurGrpList);

	//Show busy page and wait for thread to finish
	currentTask = ADD_NEURON_GROUPS_TASK;
    }
    catch (SpikeStreamException& ex){
	qCritical()<<"Error occurred adding neuron groups to network: "<<ex.getMessage();

	//Delete network from database
	if(newNetwork != NULL)
	    Globals::getNetworkDao()->deleteNetwork(newNetwork->getID());
    }
}


/*! Cancels the operation that is currently in progress. */
void NRMImportDialog::cancel(){
    operationCancelled = true;
    cout<<"CANCELLED"<<endl;
    switch (currentTask){
	case FILE_LOADING_TASK:
	    fileLoader->stop();
	break;
	case ADD_NETWORK_TASK:
	    newNetwork->cancel();
	break;
	default:
	    qCritical()<<"Current task with id " + QString::number(currentTask) + " is not recognized.";
    }

}


/*! Called when a thread doing potentially heavy operations finishes */
void NRMImportDialog::threadFinished(){
    //Operation has been cancelled by user interaction with buttons in this widget
    if(operationCancelled){
	switch (currentTask){
	    case FILE_LOADING_TASK:
		showPage1();
	    break;
	    case ADD_NEURON_GROUPS_TASK:
		showPage2();
	    break;
	    case ADD_CONNECTION_GROUPS_TASK:
		showPage2();
	    break;
	    default:
		qCritical()<<"Current task with id " + QString::number(currentTask) + " is not recognized.";
	}
    }

    //Operation has not been cancelled and thread has terminated normally
    else{
	switch (currentTask){
	    case FILE_LOADING_TASK:
		addLayersToPage2();
		showPage2();
	    break;
	    case ADD_NEURON_GROUPS_TASK:
		//Check for errors and clean up
		if(newNetwork->isError()){
		    qCritical()<<"Error adding Network: "<<newNetwork->getErrorMessage();
		    Globals::getNetworkDao()->deleteNetwork(newNetwork->getID());
		    this->reject();
		}

		//Add connection groups to network
		addConnectionGroups();

	    break;
	    case: ADD_CONNECTION_GROUPS_TASK:
		//Check for errors and clean up
		if(newNetwork->isError()){
		    qCritical()<<"Error adding Network: "<<newNetwork->getErrorMessage();
		    Globals::getNetworkDao()->deleteNetwork(newNetwork->getID());
		    this->reject();
		}
		else{
		    //Inform other classes that the list of networks has changed
		    emit networkListChanged();

		    //Inform user that it went ok
		    showSuccessPage();
		}

		//Clean up network from memory - this does not affect the network stored in the database
		if(newNetwork != NULL)
		    delete newNetwork;
	    break;
	    break;
	    default:
		qCritical()<<"Current task with id " + QString::number(currentTask) + " is not recognized.";
	}
    }

    //Reset variables
    operationCancelled = false;
    currentTask = -1;
}


/*! When user clicks on 'browse' file dialog is displayed to select the config file */
void NRMImportDialog::getConfigFile(){
    QString filePath = getFilePath("*.cfg");
    configFilePath->setText(filePath);
}


/*! When user clicks on 'browse' file dialog is displayed to select the training file */
void NRMImportDialog::getTrainingFile(){
    QString filePath = getFilePath("*.ntr");
    trainingFilePath->setText(filePath);
}


/*! Displays the busy page */
void NRMImportDialog::showBusyPage(QString busyMessage){
    busyLabel->setText(busyMessage);
    busyWidget->setVisible(true);
    page1Widget->setVisible(false);
    page2Widget->setVisible(false);
    successWidget->setVisible(false);
}


/*! Shows the first page of the wizard */
void NRMImportDialog::showPage1(){
    configFilePath->setStyleSheet("* { color: black; }");
    configFilePath->setSelection(0, 0);
    trainingFilePath->setStyleSheet("* { color: black; }");
    trainingFilePath->setSelection(0, 0);
    busyWidget->setVisible(false);
    page1Widget->setVisible(true);
    page2Widget->setVisible(false);
    successWidget->setVisible(false);
    adjustSize();
}


/*! Shows the second page of the wizard */
void NRMImportDialog::showPage2(){
    busyWidget->setVisible(false);
    page1Widget->setVisible(false);
    page2Widget->setVisible(true);
    successWidget->setVisible(false);
    adjustSize();
}


/*! Shows final page when import has been completed */
void NRMImportDialog::showSuccessPage(){
    busyWidget->setVisible(false);
    page1Widget->setVisible(false);
    page2Widget->setVisible(false);
    successWidget->setVisible(true);
}


/*! Loads up config and information from files, which is used to construct page 2. */
void NRMImportDialog::loadNetworkFromFiles(){
    bool fileError = false;
    operationCancelled = false;

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

/*! Adds connection groups and connections to the new network */
void NRMImportDialog::addConnectionGroups(){
    QList<NRMInputLayer*> inputList = fileLoader->getNetwork()->getAllInputLayers();
    QList<NRMNeuralLayer*> neuralList = fileLoader->getNetwork()->getAllNeuralLayers();

    /* Work through the list of neuron groups added to the database.
	The first items in this list are input layers, which do not contain connections.
	So calculate the offset in the list of added neuron groups. */
    int offset = inputList.size();
    for(int i=0; i<neuralList.size(); ++i){

	//Easier to manipulate the NRM neuron group
	NRMNeuralLayer* nrmNeurGrp = neuralList[i];

	//Get the list of connection groups from the NRM network
	QList<NRMConnection*> nrmConnGrpList = nrmNeurGrp->getConnections();

	for(int j=0; j<nrmConnGrpList.size(); ++j){
	    //Easier to manipulate the appropriate NRMConnection
	    NRMConnection* nrmConGrp = nrmConnGrpList[j];

	    //Create a connection group
	    ConnectionGroup* conGrp = new ConnectionGroup( ConnectionGroupInfo(id, desc, fromID, utoID,paramMap, synType) );

	    //Work through the neurons and add their connections to the group
	    for(int neurNum = 0; neurNum < nrmNeurGrp->getSize(); ++i){
		//Get the layer that this neuron connects to
		fromLayerID = nrmConGrp->srcLayerId;
		if(nrmConGrp->srcObjectType == FIXME INPUT LAYER)


		//Get the connections of this neuron
		QList<unsigned int> nrmConGrp->getNeuronConnections(neurNum);

	    }

	    //Add these connections to the network
	    newNetwork->addConnectionGroup();



	}



    }
}


/*! Adds the network layers to page 2 so that user can fine tune their
    final locations in SpikeStream */
void NRMImportDialog::addLayersToPage2(){
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


/*! Builds page to display when an operation is taking place */
void NRMImportDialog::buildBusyPage(){
    busyWidget = new QWidget(this);
    QVBoxLayout *verticalBox = new QVBoxLayout(busyWidget, 2, 2);
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
void NRMImportDialog::buildPage1(){
    page1Widget = new QWidget(this);

    //Create box to organise dialog
    QVBoxLayout *verticalBox = new QVBoxLayout(page1Widget, 2, 2);
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
    verticalBox->addLayout(gridLayout);

    //Buttons at bottom of dialog
    QHBoxLayout *buttonBox = new QHBoxLayout();
    QPushButton* cancelButton = new QPushButton("Cancel");
    buttonBox->addWidget(cancelButton);
    connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    QPushButton* nextButton = new QPushButton("Next");
    buttonBox->addWidget(nextButton);
    connect (nextButton, SIGNAL(clicked()), this, SLOT(loadNetworkFromFiles()));
    verticalBox->addLayout(buttonBox);
}


/*! Builds the second page of the widget */
void NRMImportDialog::buildPage2(){
    //Build new page 2 widget
    page2Widget = new QWidget(this);

    //Create box to organise dialog
    QVBoxLayout *verticalBox = new QVBoxLayout(page2Widget, 2, 2);
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
    QPushButton* cancelButton = new QPushButton("Cancel");
    buttonBox->addWidget(cancelButton);
    connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    QPushButton* backButton = new QPushButton("Back");
    buttonBox->addWidget(backButton);
    connect (backButton, SIGNAL(clicked()), this, SLOT(showPage1()));
    QPushButton* finishButton = new QPushButton("Finish");
    buttonBox->addWidget(finishButton);
    connect (finishButton, SIGNAL(clicked()), this, SLOT(addNetwork()));
    verticalBox->addLayout(buttonBox);
}


/*! Constructs the final page informing user that loading is complete */
void NRMImportDialog::buildSuccessPage(){
    successWidget = new QWidget(this);
    QVBoxLayout *verticalBox = new QVBoxLayout(successWidget, 2, 2);
    verticalBox->setMargin(10);
    verticalBox->addWidget(new QLabel("NRM loading complete"));
    QHBoxLayout *buttonBox = new QHBoxLayout();
    QPushButton* okButton = new QPushButton("Ok");
    buttonBox->addWidget(okButton);
    buttonBox->addStretch(5);
    connect (okButton, SIGNAL(clicked()), this, SLOT(accept()));
    verticalBox->addLayout(buttonBox);
}


/*! Enables user to enter a file path */
QString NRMImportDialog::getFilePath(QString fileFilter){
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





