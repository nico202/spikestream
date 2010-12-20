//SpikeStream includes
#include "ArchiveWidget.h"
#include "DescriptionDialog.h"
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
ArchiveWidget::ArchiveWidget(QWidget* parent) : QWidget(parent){
	QVBoxLayout* verticalBox = new QVBoxLayout(this);

	//Register types to enable signals and slots to work
	qRegisterMetaType< QList<unsigned> >("QList<unsigned>");

    //Add tool bar
    toolBar = getToolBar();
    verticalBox->addWidget(toolBar);

    //List of available archives
    gridLayout = new QGridLayout();
    gridLayout->setMargin(10);
    gridLayout->setColumnMinimumWidth(idCol, 50);//Archive ID
    gridLayout->setColumnMinimumWidth(netIDCol, 50);//NetworkID
	gridLayout->setColumnMinimumWidth(dateCol, 150);//Date and time
    gridLayout->setColumnMinimumWidth(descCol, 250);//Description

    QHBoxLayout* gridLayoutHolder = new QHBoxLayout();
    gridLayoutHolder->addLayout(gridLayout);
    gridLayoutHolder->addStretch(5);
    verticalBox->addLayout(gridLayoutHolder);

    //Load the current set of archives, if they exist, into the grid layout
    loadArchiveList();

    //Pad out with some stretch
    verticalBox->addStretch(1);

    //Listen for changes in the network and archive
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));
	connect(Globals::getEventRouter(), SIGNAL(archiveListChangedSignal()), this, SLOT(loadArchiveList()));

    //Inform other classes when a different archive is loaded
    connect(this, SIGNAL(archiveChanged()), Globals::getEventRouter(), SLOT(archiveChangedSlot()));

    //Create class to load archive data in a separate thread
    archivePlayer = new ArchivePlayerThread(Globals::getArchiveDao()->getDBInfo());
    connect(archivePlayer, SIGNAL(finished()), this, SLOT(archivePlayerStopped()));
	connect(archivePlayer, SIGNAL(timeStepChanged(unsigned, const QList<unsigned>&)), this, SLOT(updateTimeStep(unsigned, const QList<unsigned>&)), Qt::QueuedConnection);

    //Initialise variables
    ignoreButton = false;

	setMinimumHeight(200);
}


/*! Destructor */
ArchiveWidget::~ArchiveWidget(){
    delete archivePlayer;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Deletes an archive */
void ArchiveWidget::deleteArchive(){
    //Get the ID of the archive to be deleted
    unsigned int archiveID = Util::getUInt(sender()->objectName());
    if(!archiveInfoMap.contains(archiveID)){
		qCritical()<<"Archive with ID "<<archiveID<<" cannot be found.";
		return;
    }

    //Check to see if the current archive is playing
    if(Globals::isArchivePlaying() && Globals::getArchive()->getID() == archiveID){
		qWarning()<<"This current archive cannot be deleted because it is playing. Stop playback and try again.";
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
		QTimer::singleShot(500, this, SLOT(loadArchiveList()));
    }
    else{//Otherwise, just reload the archive list
		QTimer::singleShot(500, this, SLOT(loadArchiveList()));
    }
}


/*! Loads a particular archive into memory */
void ArchiveWidget::loadArchive(){
    unsigned int archiveID = sender()->objectName().toUInt();
    if(!archiveInfoMap.contains(archiveID)){
		qCritical()<<"Archive with ID "<<archiveID<<" cannot be found.";
		return;
    }

    //load the archive
    loadArchive(archiveInfoMap[archiveID]);
}


/*! Loads up a list of archives corresponding to the current network. */
void ArchiveWidget::loadArchiveList(){
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

		//Property button
		QPushButton* propButton = new QPushButton("P");
		propButton->setMaximumWidth(20);
		propButton->setObjectName(QString::number(archInfo.getID()));
		connect ( propButton, SIGNAL(clicked()), this, SLOT( setArchiveProperties() ) );
		gridLayout->addWidget(propButton, i, propCol);

		//Archive ID, network ID, date and description
		QLabel* idLabel = new QLabel(QString::number(archInfo.getID()));
		QLabel* networkIDLabel = new QLabel(QString::number(archInfo.getNetworkID()));
		QLabel* dateLabel = new QLabel(archInfo.getDateTime().toString());
		QLabel* descriptionLabel = new QLabel(archInfo.getDescription());

		//Load button and name it with the object id so we can tell which button was invoked
		QPushButton* loadButton = new QPushButton("Load");
		loadButton->setObjectName(QString::number(archInfo.getID()));
		connect ( loadButton, SIGNAL(clicked()), this, SLOT( loadArchive() ) );

		//Create the delete button and name it with the object id so we can tell which button was invoked
		QPushButton* deleteButton = new QPushButton(QIcon(Globals::getSpikeStreamRoot() + "/images/trash_can.jpg"), "");
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
		//gridLayout->setRowMinimumHeight(i, 20);
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


/*! Called when network is changed */
void ArchiveWidget::networkChanged(){
	loadArchiveList();

	//Reset time step labels
	timeStepLabel->setText("0");
	maxTimeStepLabel->setText("0");
}


/*! Rewinds the archive */
void ArchiveWidget::rewindButtonPressed(){
    //Stop thread if it is running. Rewind will be done when thread finishes
    if(archivePlayer->isRunning()){
		archivePlayer->stop();
		rewind = true;
    }
    //Archive player not running, so just rewind
    else{
		rewindArchive();
		playAction->setEnabled(true);
		stopAction->setEnabled(false);
		fastForwardAction->setEnabled(true);
    }
}


/*! Starts the archive playing. */
void ArchiveWidget::playButtonPressed(){
    //Reset variables
    rewind = false;
    step = false;
    archiveOpen = true;

    //Do nothing if no archive is loaded
    if(!Globals::archiveLoaded()){
		return;
    }

    //If archive is already playing, set the frame rate to its regular frame rate and return
    if(archivePlayer->isRunning()){
		archivePlayer->setFrameRate(Util::getUInt(frameRateCombo->currentText()));
		fastForwardAction->setEnabled(true);
		playAction->setEnabled(false);
		return;
    }

	//Update the maximum time step label in case we are playing from an archive connected to a simulator
	setMaxTimeStepLabel();

    //Start the player thread playing
    unsigned int frameRate = Util::getUInt(frameRateCombo->currentText());
    unsigned int startTimeStep = Globals::getArchive()->getTimeStep();
    archivePlayer->play(startTimeStep, Globals::getArchive()->getID(), frameRate);

	//Fix the actions
	playAction->setEnabled(false);
	fastForwardAction->setEnabled(true);
	stopAction->setEnabled(true);
}


/*! Called when the step button is pressed */
void ArchiveWidget::stepButtonPressed(){
    //Stop thread if it is running. Step will be done when thread finishes
    if(archivePlayer->isRunning()){
		archivePlayer->stop();
		step = true;
    }

    //Archive player not running, so just step
    else{
		stepArchive();
    }
}


/*! Called when the fast forward button is pressed */
void ArchiveWidget::fastForwardButtonPressed(){
    //Do nothing if no archive is loaded
    if(!Globals::archiveLoaded()){
		return;
    }

    //Set higher frame rate if archive player is already running
    if(archivePlayer->isRunning()){
		archivePlayer->setFrameRate(25);
    }
    //Otherwise start archive player with higher frame rate
    else{
		archiveOpen = true;
		unsigned int startTimeStep = Globals::getArchive()->getTimeStep();
		archivePlayer->play(startTimeStep, Globals::getArchive()->getID(), 25);
    }
	playAction->setEnabled(true);
	stopAction->setEnabled(true);
	fastForwardAction->setEnabled(false);
}


/*! Stops the archive playback */
void ArchiveWidget::stopButtonPressed(){
    archivePlayer->stop();
}


/*! Called when the frame rate combo is changed */
void ArchiveWidget::frameRateComboChanged(int){
    unsigned int frameRate = Util::getUInt(frameRateCombo->currentText());
    archivePlayer->setFrameRate(frameRate);
}


/*! Called when the player thread finishes.
    Resets the state of the buttons. */
void ArchiveWidget::archivePlayerStopped(){
    //Rewind if this has been set
    if(rewind){
		rewindArchive();
		rewind = false;
    }

    //Step if this has been set
    if(step){
		stepArchive();
		step = false;
    }

    //Reset the state of all the buttons
	playAction->setEnabled(true);
	fastForwardAction->setEnabled(true);
	stopAction->setEnabled(false);
}


/*! Sets the properties of the archive - currently just the description. */
void ArchiveWidget::setArchiveProperties(){
	unsigned int archiveID = sender()->objectName().toUInt();
	if(!archiveInfoMap.contains(archiveID)){
		qCritical()<<"Archive with ID "<<archiveID<<" cannot be found.";
		return;
	}
	try{
		DescriptionDialog* descDialog = new DescriptionDialog(archiveInfoMap[archiveID].getDescription(), this);
		if(descDialog->exec() == QDialog::Accepted){
			Globals::getArchiveDao()->setArchiveProperties(archiveID, descDialog->getDescription());
			loadArchiveList();
		}
		delete descDialog;
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Called when the time step changes and updates the time step counter */
void ArchiveWidget::updateTimeStep(unsigned timeStep, const QList<unsigned>& neuronIDList){
	//Update the time step counter and the time step in the archive
	Globals::getArchive()->setTimeStep(timeStep);
	timeStepLabel->setText(QString::number(timeStep));

	//Build new highlight map from list of IDs
	QHash<unsigned int, RGBColor*>* newHighlightMap = new QHash<unsigned int, RGBColor*>();
	RGBColor* neuronColor = Globals::getNetworkDisplay()->getArchiveFiringNeuronColor();
	foreach(unsigned tmpNeurID, neuronIDList){
		(*newHighlightMap)[tmpNeurID] = neuronColor;
	}

	//Set the colour map - this should automatically delete the old one.
	Globals::getNetworkDisplay()->setNeuronColorMap(newHighlightMap);

	//Instruct thread to continue with next time step
	archivePlayer->clearWaitForGraphics();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds the transport controls to the supplied layout */
QToolBar* ArchiveWidget::getToolBar(){
    QToolBar* tmpToolBar = new QToolBar(this);

    QAction* tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/rewind.png"), "Rewind", this);
    connect(tmpAction, SIGNAL(triggered()), this, SLOT(rewindButtonPressed()));
    tmpToolBar->addAction (tmpAction);

	playAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/play.png"), "Play", this);
	connect(playAction, SIGNAL(triggered()), this, SLOT(playButtonPressed()));
	tmpToolBar->addAction (playAction);

	tmpAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/step.png"), "Step", this);
	connect(tmpAction, SIGNAL(triggered()), this, SLOT(stepButtonPressed()));
	tmpToolBar->addAction (tmpAction);

	fastForwardAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/forward.png"), "Fast forward", this);
	connect(fastForwardAction, SIGNAL(triggered()), this, SLOT(fastForwardButtonPressed()));
	tmpToolBar->addAction (fastForwardAction);

	stopAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/stop.png"), "Stop", this);
	connect(stopAction, SIGNAL(triggered()), this, SLOT(stopButtonPressed()));
	tmpToolBar->addAction (stopAction);

    frameRateCombo = new QComboBox();
	frameRateCombo->addItem("1");
	frameRateCombo->addItem("5");
	frameRateCombo->addItem("10");
	frameRateCombo->addItem("15");
	frameRateCombo->addItem("20");
	frameRateCombo->addItem("25");
    connect(frameRateCombo, SIGNAL(activated(int)), this, SLOT(frameRateComboChanged(int)));
    tmpToolBar->addWidget(frameRateCombo);

    timeStepLabel = new QLabel ("0");
    timeStepLabel->setStyleSheet( "QLabel { margin-left: 5px; background-color: #ffffff; border-color: #555555; border-width: 2px; border-style: outset; font-weight: bold;}");
    timeStepLabel->setMinimumSize(50, 20);
    timeStepLabel->setMaximumSize(50, 20);
    timeStepLabel->setAlignment(Qt::AlignCenter);
    tmpToolBar->addWidget(timeStepLabel);
    maxTimeStepLabel = new QLabel("0");
    maxTimeStepLabel->setStyleSheet( "QLabel { margin-left: 5px; background-color: #ffffff; border-color: #555555; border-width: 2px; border-style: outset; font-weight: bold;}");
    maxTimeStepLabel->setMinimumSize(50, 20);
    maxTimeStepLabel->setMaximumSize(50, 20);
    maxTimeStepLabel->setAlignment(Qt::AlignCenter);
    tmpToolBar->addWidget(maxTimeStepLabel);

    //Disable widget and all children - will be re-enabled when an archive is loaded
    tmpToolBar->setEnabled(false);

    //Return the completed tool bar
    return tmpToolBar;
}


/*! Loads the archive */
void ArchiveWidget::loadArchive(ArchiveInfo& archiveInfo){
    if(!archiveInfoMap.contains(archiveInfo.getID())){
		qCritical()<<"Archive with ID "<<archiveInfo.getID()<<" cannot be found.";
		return;
    }

    // Create new archive
    Archive* newArchive = new Archive(archiveInfo);
    int minTimeStep = Globals::getArchiveDao()->getMinTimeStep(newArchive->getID());
    newArchive->setTimeStep(minTimeStep);
    Globals::setArchive(newArchive);

	//Inform classes about the change
    emit archiveChanged();

	//Set the time step labels
	timeStepLabel->setText( QString::number(minTimeStep) );
	setMaxTimeStepLabel();

    //Enable the transport controls
    toolBar->setEnabled(true);
	playAction->setEnabled(true);
	stopAction->setEnabled(false);
	fastForwardAction->setEnabled(true);

    //Nothing has yet been displayed of the archive firing patterns
    archiveOpen = false;

	//Schedule reloading of archive after event loop has completed
	QTimer::singleShot(500, this, SLOT(loadArchiveList()));
}


/*! Resets the state of the widget.
    Deleting the widget automatically removes it from the layout. */
void ArchiveWidget::reset(){
    //Remove no archives label if it exists
    if(archiveInfoMap.size() == 0){
    	QLayoutItem* item = gridLayout->itemAtPosition(0, 0);
		if(item != 0){
			delete item->widget();
		}
		return;
    }

    //Remove list of archives
	for(int rowIndx=0; rowIndx<archiveInfoMap.size(); ++rowIndx){
		for(int colIndx=0; colIndx < numCols; ++colIndx){
			QLayoutItem* item = gridLayout->itemAtPosition(rowIndx, colIndx);
			if(item != 0){
				item->widget()->deleteLater();
			}
		}
	}
    archiveInfoMap.clear();

    //Disable the transport controls
	if(!Globals::archiveLoaded())
		toolBar->setEnabled(false);
}


/*! Rewinds the archive back to the beginning */
void ArchiveWidget::rewindArchive(){
    if(!Globals::archiveLoaded()){
		qCritical()<<"No archive loaded,so cannot rewind archive.";
		return;
    }

	unsigned minTimeStep = Globals::getArchiveDao()->getMinTimeStep(Globals::getArchive()->getID());
	Globals::getArchive()->setTimeStep(minTimeStep);
	timeStepLabel->setText(QString::number(minTimeStep));
	Globals::getNetworkDisplay()->setNeuronColorMap(new QHash<unsigned int, RGBColor*>());
    archiveOpen = false;
}


/*! Sets the maximum time step label. */
void ArchiveWidget::setMaxTimeStepLabel(){
	if(!Globals::archiveLoaded()){
		qCritical()<<"Cannot set time step labels when no archive is loaded";
		return;
	}
	unsigned int tmpArchiveID = Globals::getArchive()->getID();
	maxTimeStepLabel->setText( QString::number(Globals::getArchiveDao()->getMaxTimeStep(tmpArchiveID)) );
}


/*! Steps the archive forward one step */
void ArchiveWidget::stepArchive(){
    if(!Globals::archiveLoaded()){
		qCritical()<<"No archive loaded,so cannot step archive.";
		return;
    }

    unsigned int startTimeStep = Globals::getArchive()->getTimeStep();
    //Step has been pressed before play, so nothing has been displayed of the archive
    if(!archiveOpen){
		archivePlayer->step(startTimeStep, Globals::getArchive()->getID());
		archiveOpen = true;
    }
    else{
		archivePlayer->step(startTimeStep+1, Globals::getArchive()->getID());
    }
}


