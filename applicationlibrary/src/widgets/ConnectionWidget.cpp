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


/*! Constructor */
ConnectionWidget::ConnectionWidget(QWidget* parent)  : QWidget(parent) {
	//Vertical box to organize layout
	QVBoxLayout* verticalBox = new QVBoxLayout(this);

	//Add button to launch add neurons dialog
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	addConnectionsButton = new QPushButton("Add Connections");
	addConnectionsButton->setMaximumSize(150, 30);
	addConnectionsButton->setEnabled(false);
	connect (addConnectionsButton, SIGNAL(clicked()), this, SLOT(addConnections()));
	buttonLayout->addWidget(addConnectionsButton);
	deleteButton = new QPushButton("Delete");
	deleteButton->setMaximumSize(150, 30);
	deleteButton->setEnabled(false);
	connect (deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelectedConnections()));
	buttonLayout->addWidget(deleteButton);
	buttonLayout->addStretch(10);
	verticalBox->addLayout(buttonLayout);

	//Construct table view and model
	connectionGroupModel = new ConnectionGroupModel();
	QTableView* connGroupView = new ConnectionGroupTableView(this, connectionGroupModel);
	verticalBox->addWidget(connGroupView);

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
	PluginsDialog* pluginsDialog = new PluginsDialog(this, "/plugins/connections", "Add Connections");
	pluginsDialog->exec();
	delete pluginsDialog;
}


/*! Deletes the selected connections */
void ConnectionWidget::deleteSelectedConnections(){
	if(!Globals::networkLoaded()){
		qCritical()<<"Attempting to delete connections when network is not loaded";
		return;
	}

	//Do nothing if no connection groups are selected
	QList<unsigned int> deleteConIDList = connectionGroupModel->getSelectedConnectionGroupIDs();
	if(deleteConIDList.isEmpty())
		return;

	//Show warning
	QMessageBox msgBox(QMessageBox::Warning, "Deleting Connections", "Are you sure that you want to delete these connection groups?\nThis step cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel, this);
	if(msgBox.exec() != QMessageBox::Ok)
		return;

	//Delete connection groups
	try{
		connect(Globals::getNetwork(), SIGNAL(taskFinished()), this, SLOT(networkTaskFinished()),  Qt::UniqueConnection);
		progressDialog = new QProgressDialog("Deleting connection group(s)", "Cancel", 0, 100, this);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(2000);
		Globals::getNetwork()->deleteConnectionGroups(deleteConIDList);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
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
void ConnectionWidget::networkTaskFinished(){
	progressDialog->setValue(100);
	progressDialog->close();
	//delete progressDialog;

	//Clear selection on connection group model
	connectionGroupModel->clearSelection();

	//Prevent this method being called when network finishes other tasks
	this->disconnect(Globals::getNetwork(), SIGNAL(taskFinished()));

	//Inform other classes that network has changed
	Globals::getEventRouter()->networkChangedSlot();
}

