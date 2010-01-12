//SpikeStream includes
#include "ArchiveWidget_V2.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>

/*! Constructor */
ArchiveWidget_V2::ArchiveWidget_V2(QWidget* parent) : QWidget(parent){
    QVBoxLayout* verticalBox = new QVBoxLayout(this, 2, 2);

    //Add controls to play the loaded archive
    buildTransportControls();
    verticalBox->addWidget(transportControlWidget);
    verticalBox->addSpacing(10);

    gridLayout = new QGridLayout();
    gridLayout->setMargin(10);
    gridLayout->setColumnMinimumWidth(idCol, 50);//Archive ID
    gridLayout->setColumnMinimumWidth(netIDCol, 50);//NetworkID
    gridLayout->setColumnMinimumWidth(dateCol, 100);//Date and time
    gridLayout->setColumnMinimumWidth(descCol, 250);//Description
    gridLayout->setColumnMinimumWidth(loadButCol, 100);//Load button
    gridLayout->setColumnMinimumWidth(delButCol, 100);//Delete button

    QHBoxLayout* gridLayoutHolder = new QHBoxLayout();
    gridLayoutHolder->addLayout(gridLayout);
    gridLayoutHolder->addStretch(5);
    verticalBox->addLayout(gridLayoutHolder);

    //Load the current set of archives, if they exist, into the grid layout
    loadArchiveList();

    //Pad out with some stretch
    verticalBox->addStretch(1);

    //Listen for changes in the network and archive
    connect(Globals::getEventRouter(), SIGNAL(archiveChangedSignal()), this, SLOT(loadArchiveList()));
    connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(loadArchiveList()));

    //Inform other classes when a different archive is loaded
    connect(this, SIGNAL(archiveChanged()), Globals::getEventRouter(), SLOT(archiveChangedSlot()));

    //Listen for changes in the archive time step
    connect(Globals::getEventRouter(), SIGNAL(archiveTimeStepChangedSignal()),this, SLOT(archiveTimeStepChanged()));

    //Create class to load archive data in a separate thread
    archivePlayer = new ArchivePlayerThread(Globals::getArchiveDao()->getDBInfo());
    connect(archivePlayer, SIGNAL(finished()), this, SLOT(archivePlayerStopped()));

    //Inform other classes when the archive time step changes
    connect(archivePlayer, SIGNAL(archiveTimeStepChanged()), Globals::getEventRouter(), SLOT(archiveTimeStepChangedSlot()));

    //Initialise variables
    ignoreButton = false;
}


/*! Destructor */
ArchiveWidget_V2::~ArchiveWidget_V2(){
    delete archivePlayer;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Deletes an archive */
void ArchiveWidget_V2::deleteArchive(){
    //Get the ID of the archive to be deleted
    unsigned int archiveID = Util::getUInt(sender()->objectName());
    if(!archiveInfoMap.contains(archiveID)){
	qCritical()<<"Archive with ID "<<archiveID<<" cannot be found.";
	return;
    }

    //Confirm that user wants to take this action.
    QMessageBox msgBox;
    msgBox.setText("Deleting Archive");
    msgBox.setInformativeText("Are you sure that you want to delete archive with ID " + QString::number(archiveID) + "? This step cannot be undone.");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret != QMessageBox::Ok)
	return;

    //Delete the archive from the database
    try{
	Globals::getArchiveDao()->deleteArchive(archiveID);
    }
    catch(SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
    }

    /* If we have deleted the current archive, use event router to inform other classes that the archive has changed.
       This will automatically reload the archive list. */
    if(Globals::archiveLoaded() && Globals::getArchive()->getID() == archiveID){
	Globals::setArchive(NULL);
	emit archiveChanged();
    }
    else{//Otherwise, just reload the archive list
	loadArchiveList();
    }
}


/*! Loads a particular archive into memory */
void ArchiveWidget_V2::loadArchive(){
    unsigned int archiveID = sender()->objectName().toUInt();
    if(!archiveInfoMap.contains(archiveID)){
	qCritical()<<"Archive with ID "<<archiveID<<" cannot be found.";
	return;
    }

    //load the archive
    loadArchive(archiveInfoMap[archiveID]);
}


/*! Loads up a list of archives corresponding to the current network. */
void ArchiveWidget_V2::loadArchiveList(){
    //Reset widget
    reset();

    //If no network is loaded, show no network loaded message and return
    if(!Globals::networkLoaded()){
	gridLayout->addWidget(new QLabel("No network loaded."), 0, 0);
	return;
    }

    //Get a list of the archives in the database
    ArchiveDao* archiveDao = Globals::getArchiveDao();
    QList<ArchiveInfo> archiveInfoList;
    try{
	archiveInfoList = archiveDao->getArchivesInfo(Globals::getNetwork()->getID());
    }
    catch(SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
	return;
    }

    //Show "no archive" message if list is empty
    if(archiveInfoList.size() == 0){
	gridLayout->addWidget(new QLabel("No archives in database"), 0, 0);
    }

    //Copy archive infos into map
    for(int i=0; i<archiveInfoList.size(); ++i){
	archiveInfoMap[archiveInfoList[i].getID()] = archiveInfoList[i];
    }

    /* If the current archive is in the archive list, then set this as the one loaded
       Otherwise currentArchiveID is set to zero and the user has to choose the loaded archive */
    unsigned int currentArchiveID = 0;
    if(Globals::archiveLoaded() && archiveInfoMap.contains(Globals::getArchive()->getID())){
	currentArchiveID = Globals::getArchive()->getID();
    }


    //Display the list in the widget
    for(int i=0; i<archiveInfoList.size(); ++i){
	ArchiveInfo archInfo = archiveInfoList[i];

	//Create labels
	QLabel* idLabel = new QLabel(QString::number(archInfo.getID()));
	QLabel* networkIDLabel = new QLabel(QString::number(archInfo.getNetworkID()));
	QLabel* dateLabel = new QLabel(archInfo.getDateTime().toString());
	QLabel* descriptionLabel = new QLabel(archInfo.getDescription());

	//Create the load button and name it with the object id so we can tell which button was invoked
	QPushButton* loadButton = new QPushButton("Load");
	loadButton->setObjectName(QString::number(archInfo.getID()));
	connect ( loadButton, SIGNAL(clicked()), this, SLOT( loadArchive() ) );

	//Create the delete button and name it with the object id so we can tell which button was invoked
	QPushButton* deleteButton = new QPushButton("Delete");
	deleteButton->setObjectName(QString::number(archInfo.getID()));
	connect ( deleteButton, SIGNAL(clicked()), this, SLOT( deleteArchive() ) );

	//Set labels and buttons depending on whether it is the current archive
	if(currentArchiveID == archInfo.getID()){//The curently loaded archive
	    idLabel->setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
	    networkIDLabel->setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
	    dateLabel->setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
	    descriptionLabel->setStyleSheet( "QLabel { color: #008000; font-weight: bold; }");
	    loadButton->setEnabled(false);
	}
	else{//An archive that is not loaded
	    idLabel->setStyleSheet( "QLabel { color: #777777; }");
	    networkIDLabel->setStyleSheet( "QLabel { color: #777777; }");
	    dateLabel->setStyleSheet( "QLabel { color: #777777; }");
	    descriptionLabel->setStyleSheet( "QLabel { color: #777777; }");
	}

	//Add the widgets to the layout
	gridLayout->addWidget(idLabel, i, idCol);
	gridLayout->addWidget(networkIDLabel, i, netIDCol);
	gridLayout->addWidget(dateLabel, i, dateCol);
	gridLayout->addWidget(descriptionLabel, i, descCol);
	gridLayout->addWidget(loadButton, i, loadButCol);
	gridLayout->addWidget(deleteButton, i, delButCol);
	gridLayout->setRowMinimumHeight(i, 20);
    }


    //List of archives is empty or current archive is not found in this list
    if(archiveInfoList.size() == 0 || currentArchiveID == 0){
	//Unload the current archive and inform other classes
	if(Globals::archiveLoaded()){
	    Globals::setArchive(0);
	    emit archiveChanged();
	}
    }

    //FIXME: HACK TO GET IT TO DISPLAY PROPERLY
    this->setMinimumHeight(archiveInfoList.size() * 100);
}


/*! Called when the time step changes and updates the time step counter */
void ArchiveWidget_V2::archiveTimeStepChanged(){
    //Update the time step counter
    timeStepLabel->setText(QString::number(Globals::getArchive()->getTimeStep()));
}


void ArchiveWidget_V2::rewindButtonPressed(){
    //Stop thread if it is running. Rewind will be done when thread finishes
    if(archivePlayer->isRunning()){
	archivePlayer->stop();
	rewind = true;
    }
    //Archive player not running, so just rewind
    else{
	rewindArchive();
    }
}

void ArchiveWidget_V2::playButtonToggled(bool on){
    if(ignoreButton){
	ignoreButton = false;
	return;
    }

    //Reset variables
    rewind = false;

    //Reset play button if no archive is loaded
    if(!Globals::archiveLoaded()){
	ignoreButton= true;
	playButton->toggle();
	return;
    }

    if(on){
	//Start the player thread playing
	unsigned int frameRate = Util::getUInt(frameRateCombo->currentText());
	unsigned int startTimeStep = Globals::getArchive()->getTimeStep();
	archivePlayer->play(startTimeStep, Globals::getArchive()->getID(), frameRate);

	//FIXME: DISABLE EDITING AND LOADING FUNCTIONS

	if(fastForwardButton->isOn()){
	    ignoreButton = true;
	    fastForwardButton->toggle();
	}
    }
    else{
	archivePlayer->stop();
    }
}



void ArchiveWidget_V2::stepButtonPressed(){
}
void ArchiveWidget_V2::fastForwardButtonToggled(bool on){
}

void ArchiveWidget_V2::stopButtonPressed(){
    archivePlayer->stop();
}


/*! Called when the frame rate combo is changed */
void ArchiveWidget_V2::frameRateComboChanged(int){
    unsigned int frameRate = Util::getUInt(frameRateCombo->currentText());
    archivePlayer->setFrameRate(frameRate);
}

void ArchiveWidget_V2::rewindArchive(){
    if(!Globals::archiveLoaded()){
	qCritical()<<"No archive loaded,so cannot rewind archive.";
	return;
    }

    Globals::getArchive()->setTimeStep(0);
    Globals::getEventRouter()->archiveTimeStepChangedSlot();
    Globals::getNetworkDisplay()->clearNeuronColorMap();
}


/*! Called when the player thread finishes.
    Resets the state of the buttons. */
void ArchiveWidget_V2::archivePlayerStopped(){
    //Rewind if this has been set
    if(rewind){
	rewindArchive();
	rewind = false;
    }

    //Reset the state of all the buttons
    if(fastForwardButton->isOn()){
	ignoreButton = true;
	fastForwardButton->toggle();
    }
    if(playButton->isOn()){
	ignoreButton = true;
	playButton->toggle();
    }
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds the transport controls to the supplied layout */
void ArchiveWidget_V2::buildTransportControls(){
    //Set up pixmaps to control play and stop
    QPixmap playPixmap(Globals::getSpikeStreamRoot() + "/images/play.xpm");
    QPixmap stepPixmap(Globals::getSpikeStreamRoot() + "/images/step.xpm");
    QPixmap stopPixmap(Globals::getSpikeStreamRoot() + "/images/stop.xpm");
    QPixmap rewindPixmap(Globals::getSpikeStreamRoot() + "/images/rewind.xpm");
    QPixmap fastForwardPixmap(Globals::getSpikeStreamRoot() + "/images/fast_forward.xpm");

    transportControlWidget = new QWidget(this);
    QHBoxLayout *archTransportBox = new QHBoxLayout(transportControlWidget);
    QPushButton* rewindButton = new QPushButton(QIcon(rewindPixmap), "");
    rewindButton->setBaseSize(30, 30);
    rewindButton->setMaximumSize(30, 30);
    rewindButton->setMinimumSize(30, 30);
    connect (rewindButton, SIGNAL(clicked()), this, SLOT(rewindButtonPressed()));
    archTransportBox->addSpacing(10);
    archTransportBox->addWidget(rewindButton);

    playButton = new QPushButton(QIcon(playPixmap), "");
    playButton->setToggleButton(true);
    playButton->setBaseSize(100, 30);
    playButton->setMaximumSize(100, 30);
    playButton->setMinimumSize(100, 30);
    connect (playButton, SIGNAL(toggled(bool)), this, SLOT(playButtonToggled(bool)));
    archTransportBox->addWidget(playButton);

    stepButton = new QPushButton(QIcon(stepPixmap), "");
    stepButton->setBaseSize(50, 30);
    stepButton->setMaximumSize(50, 30);
    stepButton->setMinimumSize(50, 30);
    connect (stepButton, SIGNAL(clicked()), this, SLOT(stepButtonPressed()));
    archTransportBox->addWidget(stepButton);

    fastForwardButton = new QPushButton(QIcon(fastForwardPixmap), "");
    fastForwardButton->setToggleButton(true);
    fastForwardButton->setBaseSize(30, 30);
    fastForwardButton->setMaximumSize(30, 30);
    fastForwardButton->setMinimumSize(30, 30);
    connect (fastForwardButton, SIGNAL(toggled(bool)), this, SLOT(fastForwardButtonToggled(bool)));
    archTransportBox->addWidget(fastForwardButton);

    stopButton = new QPushButton(QIcon(stopPixmap), "");
    stopButton->setBaseSize(50, 30);
    stopButton->setMaximumSize(50, 30);
    stopButton->setMinimumSize(50, 30);
    connect (stopButton, SIGNAL(clicked()), this, SLOT(stopButtonPressed()));
    archTransportBox->addWidget(stopButton);
    archTransportBox->addSpacing(10);

    archTransportBox->addWidget( new QLabel("Frames per second") );
    frameRateCombo = new QComboBox();
    frameRateCombo->insertItem("1");
    frameRateCombo->insertItem("5");
    frameRateCombo->insertItem("10");
    frameRateCombo->insertItem("15");
    frameRateCombo->insertItem("20");
    frameRateCombo->insertItem("25");
    connect(frameRateCombo, SIGNAL(activated(int)), this, SLOT(frameRateComboChanged(int)));
    archTransportBox->addWidget(frameRateCombo);

    //FIXME THESE LOOK REALLY CRAPPY HERE - REPLACE WITH A PROPER TOOLBAR
    timeStepLabel = new QLabel ("0");
    timeStepLabel->setStyleSheet( "QLabel { background-color: #ffffff; border-color: #555555; border-width: 2px; border-style: outset; font-weight: bold;}");
    timeStepLabel->setMinimumSize(50, 20);
    timeStepLabel->setAlignment(Qt::AlignCenter);
    archTransportBox->addWidget(timeStepLabel);
    maxTimeStepLabel = new QLabel("0");
    maxTimeStepLabel->setStyleSheet( "QLabel { background-color: #ffffff; border-color: #555555; border-width: 2px; border-style: outset; font-weight: bold;}");
    maxTimeStepLabel->setMinimumSize(50, 20);
    maxTimeStepLabel->setAlignment(Qt::AlignCenter);
    archTransportBox->addWidget(maxTimeStepLabel);

    archTransportBox->addStretch(5);

    //Disable widget and all children - will be re-enabled when an archive is loaded
    transportControlWidget->setEnabled(false);
}


/*! Loads the archive */
void ArchiveWidget_V2::loadArchive(ArchiveInfo& archiveInfo){
    if(!archiveInfoMap.contains(archiveInfo.getID())){
	qCritical()<<"Archive with ID "<<archiveInfo.getID()<<" cannot be found.";
	return;
    }

    // Create new archive
    Archive* newArchive = new Archive(archiveInfo);
    Globals::setArchive(newArchive);

    //Inform classes about the change - this should trigger reloading of archive list via event router
    emit archiveChanged();

    //Set the time step labels
    timeStepLabel->setText(QString::number(Globals::getArchiveDao()->getMinTimeStep(newArchive->getID())));
    maxTimeStepLabel->setText(QString::number(Globals::getArchiveDao()->getMaxTimeStep(newArchive->getID())));

    //Enable the transport controls
    transportControlWidget->setEnabled(true);
}


/*! Resets the state of the widget.
    Deleting the widget automatically removes it from the layout. */
void ArchiveWidget_V2::reset(){
    //Remove no archives label if it exists
    if(archiveInfoMap.size() == 0){
    	QLayoutItem* item = gridLayout->itemAtPosition(0, 0);
	if(item != 0){
	    delete item->widget();
	}
	return;
    }

    //Remove list of archives
    for(int i=0; i<archiveInfoMap.size(); ++i){
	QLayoutItem* item = gridLayout->itemAtPosition(i, idCol);
	if(item != 0){
	    item->widget()->deleteLater();
	}
	item = gridLayout->itemAtPosition(i, netIDCol);
	if(item != 0){
	    item->widget()->deleteLater();
	}
	item = gridLayout->itemAtPosition(i, dateCol);
	if(item != 0){
	    item->widget()->deleteLater();
	}
	item = gridLayout->itemAtPosition(i, descCol);
	if(item != 0){
	    item->widget()->deleteLater();
	}
	item = gridLayout->itemAtPosition(i, loadButCol);
	if(item != 0){
	    item->widget()->deleteLater();
	}
	item = gridLayout->itemAtPosition(i, delButCol);
	if(item != 0){
	    item->widget()->deleteLater();
	}
    }
    archiveInfoMap.clear();

    //Disable the transport controls
    transportControlWidget->setEnabled(false);
}

