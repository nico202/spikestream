//SpikeStream includes
#include "NRMImportDialog.h"
#include "Globals.h"
#include "NRMException.h"
#include "Box.h"
#include "NetworkDao.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QProgressBar>

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
    dataImporter = new NRMDataImporter(Globals::getNetworkDao()->getDBInfo());
    connect (dataImporter, SIGNAL(finished()), this, SLOT(threadFinished()));

    //Set initial state of variables
    dataImporting = false;
    fileLoading = false;
    operationCancelled = false;

    //Show the first page
    showPage1();
}


/*! Destructor */
NRMImportDialog::~NRMImportDialog(){
    delete fileLoader;
    delete dataImporter;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Builds the network based on the information stored in the dialog and entered by the user */
void NRMImportDialog::addNetwork(){
    //Validate the input fields - should contain integers

    //

}


/*! Cancels the operation that is currently in progress. */
void NRMImportDialog::cancel(){
    operationCancelled = true;
    if(fileLoading)
	fileLoader->stop();
    else if (dataImporting)
	dataImporter->stop();
}


/*! Called when a thread doing potentially heavy operations finishes */
void NRMImportDialog::threadFinished(){
    //Operation has been cancelled by user interaction with buttons in this widget
    if(operationCancelled){
	if(fileLoading){
	    showPage1();
	}
	else if(dataImporting){
	    showPage2();
	}
	else
	    qCritical()<<"Either fileLoading or dataLoading should be set.";
    }

    //Operation has not been cancelled and thread has terminated normally
    else{
	if(fileLoading){
	    addLayersToPage2();
	    showPage2();
	}
	else if(dataImporting){
	    showSuccessPage();
	}
	else
	    qCritical()<<"Either fileLoading or dataLoading should be set.";
    }

    //Reset variables
    operationCancelled = false;
    fileLoading = false;
    dataImporting = false;
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
}


/*! Shows the second page of the wizard */
void NRMImportDialog::showPage2(){
    busyWidget->setVisible(false);
    page1Widget->setVisible(false);
    page2Widget->setVisible(true);
    successWidget->setVisible(false);
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
    //Check that file paths are valid
    bool fileError = false;
    if(!QFile::exists(configFilePath->text())){
	if(configFilePath->text() == "")
	    configFilePath->setText("Please enter a valid file path");
	configFilePath->setStyleSheet("* { color: red; }");
	configFilePath->setSelection(0, 0);
	fileError = true;
    }
    if(!QFile::exists(trainingFilePath->text())){
	if(trainingFilePath->text() == "")
	    trainingFilePath->setText("Please enter a valid file path");
	trainingFilePath->setStyleSheet("* { color: red; }");
	trainingFilePath->setSelection(0, 0);
	fileError = true;
    }
    if(fileError)
	return;

    //Load up configuration file
    showBusyPage(QString ("Loading configuration and training files"));
    try{
	operationCancelled =false;
	fileLoading = true;
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

    /* Default option is to place layers in a column with a spacing of ten between each layer
	Start by finding the maximum width and heigth of a layer */
    unsigned int maxWidth = 0, maxHeight = 0, layerCount = 0;
    for(int i=0; i<inputList.size(); ++i){
	++layerCount;
	if(inputList[i]->width > maxWidth)
	    maxWidth = inputList[i]->width;
	if(inputList[i]->height > maxHeight)
	    maxHeight = inputList[i]->height;
    }
    for(int i=0; i<neuralList.size(); ++i){
	++layerCount;
	if(neuralList[i]->width > maxWidth)
	    maxWidth = neuralList[i]->width;
	if(neuralList[i]->height > maxHeight)
	    maxHeight = neuralList[i]->height;
    }

    //Check that there are no neurons in the proposed import volume
    Box box(0, 0, 0, maxWidth, maxHeight, layerCount*10);
    unsigned int neurCount = 1;
   /* while(neurCount != 0){
	neurCount = Globals::getNetworkDao()->countNeuronsInVolume(box);
	if(neurCount != 0){
	    box.translate(10, 0, 0);
	}
    }*/

    //Add input layers to grid layout.
    int rowCount = 0;
    for(int i=0; i<inputList.size(); ++i){
	layerLocationGrid->addWidget(new QLabel( QString::number(inputList[i]->frameNum) ), rowCount, 0);
	layerLocationGrid->addWidget(new QLabel( inputList[i]->frameName.data() ), rowCount, 1);
	QString sizeStr = "(" + QString::number(inputList[i]->width) + "x" + QString::number(inputList[i]->height) + "):";
	layerLocationGrid->addWidget(new QLabel(sizeStr), rowCount, 2);//size
	layerLocationGrid->addWidget(new QLabel(" x="), rowCount, 3);//size
	layerLocationGrid->addWidget(new QLineEdit( QString::number(box.getX1()) ), rowCount, 4);//x location
	layerLocationGrid->addWidget(new QLabel(" y="), rowCount, 5);//size
	layerLocationGrid->addWidget(new QLineEdit( QString::number(box.getY1()) ), rowCount, 6);//y location
	layerLocationGrid->addWidget(new QLabel(" z="), rowCount, 7);//size
	layerLocationGrid->addWidget(new QLineEdit( QString::number(box.getZ1()) ), rowCount, 8);//z location
	++rowCount;
    }

    //Add neural layers to grid layout.
    for(int i=0; i<neuralList.size(); ++i){
	layerLocationGrid->addWidget(new QLabel( QString::number(neuralList[i]->frameNum) ), rowCount, 0);
	layerLocationGrid->addWidget(new QLabel( neuralList[i]->frameName.data() ), rowCount, 1);
	QString sizeStr = "(" + QString::number(neuralList[i]->width) + "x" + QString::number(neuralList[i]->height) + "):";
	layerLocationGrid->addWidget(new QLabel(sizeStr), rowCount, 2);//size
	layerLocationGrid->addWidget(new QLabel(" x="), rowCount, 3);//size
	layerLocationGrid->addWidget(new QLineEdit( QString::number(box.getX1()) ), rowCount, 4);//x location
	layerLocationGrid->addWidget(new QLabel(" y="), rowCount, 5);//size
	layerLocationGrid->addWidget(new QLineEdit( QString::number(box.getY1()) ), rowCount, 6);//y location
	layerLocationGrid->addWidget(new QLabel(" z="), rowCount, 7);//size
	layerLocationGrid->addWidget(new QLineEdit( QString::number(box.getZ1()) ), rowCount, 8);//z location
	++rowCount;
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
    verticalBox->addLayout(layerLocationGrid);
    verticalBox->addSpacing(10);

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


void NRMImportDialog::buildSuccessPage(){
    successWidget = new QWidget(this);
    QVBoxLayout *verticalBox = new QVBoxLayout(successWidget, 2, 2);
    verticalBox->setMargin(10);
    verticalBox->addWidget(new QLabel("NRM loading complete"));
    QHBoxLayout *buttonBox = new QHBoxLayout();
    QPushButton* okButton = new QPushButton("Ok");
    buttonBox->addWidget(okButton);
    connect (okButton, SIGNAL(clicked()), this, SLOT(accept()));
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



