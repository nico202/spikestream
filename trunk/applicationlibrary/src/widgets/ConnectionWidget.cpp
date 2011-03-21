//SpikeStream includes
#include "ConnectionWidget.h"
#include "ConnectionGroupTableView.h"
#include "Globals.h"
#include "PluginsDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QMessageBox>
#include <QMutexLocker>


/*! Constructor */
ConnectionWidget::ConnectionWidget(QWidget* parent)  : QWidget(parent) {
	//Vertical box to organize layout
	QVBoxLayout* verticalBox = new QVBoxLayout(this);

	//Add button to launch add neurons dialog
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	addConnectionsButton = new QPushButton(" Add Connections ");
	addConnectionsButton->setEnabled(false);
	connect (addConnectionsButton, SIGNAL(clicked()), this, SLOT(addConnections()));
	buttonLayout->addWidget(addConnectionsButton);
	deleteButton = new QPushButton(QIcon(Globals::getSpikeStreamRoot() + "/images/trash_can.png"), "");
	deleteButton->setEnabled(false);
	connect (deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelectedConnections()));
	buttonLayout->addWidget(deleteButton);
	buttonLayout->addStretch(10);
	verticalBox->addLayout(buttonLayout);

	//Construct table view and model
	connectionGroupModel = new ConnectionGroupModel();
	QTableView* connGroupView = new ConnectionGroupTableView(this, connectionGroupModel);
	verticalBox->addWidget(connGroupView);

	//Connection manager to delete connections in a separate thread
	connectionManager = new ConnectionManager();
	connect(connectionManager, SIGNAL(progress(int,int,QString)), this, SLOT(updateProgress(int,int,QString)), Qt::QueuedConnection);
	connect(connectionManager, SIGNAL(finished()), this, SLOT(connectionManagerFinished()));

	//Enable or disable buttons depending on whether a network is loaded or not
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));
}


/*! Destructor */
ConnectionWidget::~ConnectionWidget(){
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Shows plugins dialog configured to load up available plugins for adding neurons */
void ConnectionWidget::addConnections(){
	//Run some checks - cannot change network if simulation or analysis is going on.
	if(!Globals::networkChangeOk())
		return;
	PluginsDialog* pluginsDialog = new PluginsDialog(this, "/plugins/connections", "Add Connections");
	pluginsDialog->exec();
	delete pluginsDialog;
}


/*! Deletes the selected connections */
void ConnectionWidget::deleteSelectedConnections(){
	//Runs some checks - cannot change network if simulation or analysis is going on.
	if(!Globals::networkLoaded()){
		qCritical()<<"Attempting to delete connections when network is not loaded";
		return;
	}
	//Check if network is editable or not
	if(Globals::getNetwork()->hasArchives()){
		qCritical()<<"You cannot alter a network that has archives.\nDelete the archives and try again.";
		return;
	}

	if(!Globals::networkChangeOk())
		return;

	//Do nothing if no connection groups are selected
	QList<unsigned> deleteConnectionIDList = connectionGroupModel->getSelectedConnectionGroupIDs();
	if(deleteConnectionIDList.isEmpty())
		return;

	//Show warning
	QMessageBox msgBox(QMessageBox::Warning, "Deleting Connections", "Are you sure that you want to delete these connection groups?\nThis step cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel, this);
	if(msgBox.exec() != QMessageBox::Ok)
		return;

	//Delete connection groups
	try{
		progressDialog = new QProgressDialog("Deleting connection group(s)", "Cancel", 0, 0, this, Qt::CustomizeWindowHint);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setCancelButton(0);//Too complicated to implement cancel sensibly
		progressDialog->show();
		updatingProgress = false;
		connectionManager->deleteConnectionGroups(deleteConnectionIDList);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Updates user with feedback about progress with the operation */
void ConnectionWidget::updateProgress(int stepsCompleted, int totalSteps, QString message){
	//Set flag to avoid multiple calls to progress dialog while it is redrawing
	if(updatingProgress)
		return;
	updatingProgress = true;

	//Check for cancellation
	if(progressDialog->wasCanceled()){
		qCritical()<<"SpikeStream does not currently support cancellation of deleting connections.";
	}

	//Update progress
	else if(stepsCompleted < totalSteps){
		progressDialog->setValue(stepsCompleted);
		progressDialog->setMaximum(totalSteps);
		progressDialog->setLabelText(message);
	}

	//Progress has finished
	else{
		progressDialog->close();
	}

	//Clear flag to indicate that update of progress is complete
	updatingProgress = false;
}


/*! Enables or disables buttons depending on whether a network is loaded */
void ConnectionWidget::networkChanged(){
	if(Globals::networkLoaded() && (Globals::getNetwork()->getNeuronGroupCount() > 0) ){
		addConnectionsButton->setEnabled(true);
		deleteButton->setEnabled(true);
	}
	else{
		addConnectionsButton->setEnabled(false);
		deleteButton->setEnabled(false);
	}
}


/*! Called when the network has finished deleting neurons.
	Informs other classes that network has changed. */
void ConnectionWidget::connectionManagerFinished(){
	//Check for errors
	if(connectionManager->isError())
		qCritical()<<connectionManager->getErrorMessage();

	//Clean up progress dialog
	if(progressDialog != NULL){
		progressDialog->close();
		delete progressDialog;
		progressDialog = NULL;
	}

	//Clear selection on connection group model
	connectionGroupModel->clearSelection();

	//Inform other classes that network has changed
	Globals::getEventRouter()->networkChangedSlot();
}

