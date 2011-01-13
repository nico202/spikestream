//SpikeStream includes
#include "EventRouter.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "IzhikevichNetworksWidget.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QDialog>

//Other includes
#include <typeinfo>

//Functions for dynamic library loading
extern "C" {
    /*! Creates a StateBasedPhiWidget class when library is dynamically loaded. */
	IzhikevichNetworksWidget* getClass(){
		return new IzhikevichNetworksWidget();
    }

    /*! Returns a descriptive name for this widget */
    QString getName(){
		return QString("Izhikevich Networks");
    }
}


/*! Constructor */
IzhikevichNetworksWidget::IzhikevichNetworksWidget(){
    //Create class that will be used to build networks
	networkBuilder = new IzhikevichNetworkBuilder();
	connect(networkBuilder, SIGNAL( progress(int, int) ), this, SLOT( updateProgress(int, int) ) );
	connect(networkBuilder, SIGNAL( finished() ), this, SLOT( networkBuilderFinished() ) );

	//Set up progress dialog
	progressDialog = new QProgressDialog(this);
	progressDialog->setAutoClose(false);
	progressDialog->setAutoReset(false);
	progressDialog->setModal(true);
	progressDialog->setMinimumDuration(0);

    //Layout to manage whole widget
    QVBoxLayout* mainVBox = new QVBoxLayout(this);

    //Layout using grid layout
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setMargin(10);

    //Field to enable user to enter network name
    gridLayout->addWidget(new QLabel("Network name: "), 0, 0);
    networkName = new QLineEdit("Unnamed");
    networkName->setMinimumSize(250, 30);
    gridLayout->addWidget(networkName, 0, 1);

    //Add buttons for each network
	QPushButton* newButton = addNetworkButton(gridLayout, "2006 Polychronization Network");
    connect (newButton, SIGNAL(clicked()), this, SLOT(addNetwork()));

    mainVBox->addLayout(gridLayout);
    mainVBox->addStretch(5);

}


/*! Destructor */
IzhikevichNetworksWidget::~IzhikevichNetworksWidget(){
}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Adds a network matching the name of the sender to the database */
void IzhikevichNetworksWidget::addNetwork(){
	//Check to see if another network is loaded and not saved.
	if(Globals::networkLoaded() && !Globals::getNetwork()->isSaved()){
		qWarning()<<"This plugin unloads the current network and creates a new network in memory.\nIt cannot continue until the current network is saved or discarded.";
		return;
	}

    QString netDesc = sender()->objectName();
    try{
		if(netDesc == "2006 Polychronization Network"){
			//Feedback on progress
			progressDialog->show();
			progressUpdating = false;

			//Create network - need to create this outside of thread to avoid event handling problems.
			newNetwork = new Network(networkNameEdit->text(), networkNameEdit->text(), Globals::getNetworkDao()->getDBInfo(), Globals::getArchiveDao()->getDBInfo());
			newNetwork->setPrototypeMode(true);
			newNetwork->setTransient(true);//Network will be deleted on close if it has not been saved
			networkBuilder->startAddPolychronizationNetwork(newNetwork);
		}
		else
			throw SpikeStreamException("Network descrption not recognized: " + netDesc);

		Globals::getEventRouter()->reloadSlot();
    }
    catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
    }
}


/*! Called when the network builder finishes its current task.
	Hides progress dialog and initiates reload of list of networks  */
void IzhikevichNetworksWidget::networkBuilderFinished(){
	progressDialog->hide();

	//Show error
	if(networkBuilder->isError()){
		qCritical()<<networkBuilder->getErrorMessage();
	}

	//Delete network if there was an error or the operation was cancelled
	if(networkBuilder->isError() || progressDialog->wasCanceled()){
		if(newNetwork != NULL){
			NetworkDaoThread netDaoThread(Globals::getNetworkDao()->getDBInfo());
			netDaoThread.startDeleteNetwork(newNetwork->getID());
			netDaoThread.wait();
			if(netDaoThread.isError())
				qCritical()<<netDaoThread.getErrorMessage();
			delete newNetwork;
		}
	}
	else{
		//Make the network the current network
		Globals::setNetwork(newNetwork);
		Globals::getEventRouter()->networkChangedSlot();
	}
}


void IzhikevichNetworksWidget::updateProgress(int stepsCompleted, int totalSteps, QString message){
	//Avoid multiple calls to graphics
	if(progressUpdating)
		return;

	progressUpdating = true;

	if(progressDialog->wasCanceled()){
		networkBuilder->cancel();
		progressDialog->show();
	}

	if(networkBuilder->isRunning() && progressDialog->isVisible()){
		progressDialog->setMaximum(totalSteps);
		progressDialog->setValue(stepsCompleted);
		progressDialog->setLabelText(message);
	}

	progressUpdating = false;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Returns the name of the network to be added, or 'Unnamed' if this is not specified. */
QString IzhikevichNetworksWidget::getNetworkName(){
    if(networkName->text() == "")
	return QString("Unnamed");
    return networkName->text();
}


/*! Adds a button for adding a network to the GUI */
QPushButton* IzhikevichNetworksWidget::addNetworkButton(QGridLayout* gridLayout, const QString& description){
    int row = gridLayout->rowCount();
    gridLayout->addWidget(new QLabel(description), row, 0);
    QPushButton* addButton = new QPushButton("Add");
    addButton->setObjectName(description);
    addButton->setMaximumSize(120, 30);
    gridLayout->addWidget(addButton, row, 1);
    return addButton;
}


