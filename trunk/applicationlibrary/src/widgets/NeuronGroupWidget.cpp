//SpikeStream includes
#include "Globals.h"
#include "NeuronGroupWidget.h"
#include "NeuronGroupTableView.h"
#include "PluginsDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QMessageBox>
#include <QLayout>
#include <QTableView>


/*! Constructor */
NeuronGroupWidget::NeuronGroupWidget(QWidget* parent)  : QWidget(parent) {
	//Vertical box to organize layout
	QVBoxLayout* verticalBox = new QVBoxLayout(this);

	//Add button to launch add neurons dialog and button to delete neuron groups
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	addNeuronsButton = new QPushButton(" Add Neurons ");
	addNeuronsButton->setEnabled(false);
	connect (addNeuronsButton, SIGNAL(clicked()), this, SLOT(addNeurons()));
	buttonLayout->addWidget(addNeuronsButton);
	deleteButton = new QPushButton(QIcon(Globals::getSpikeStreamRoot() + "/images/trash_can.png"), "");
	deleteButton->setMaximumSize(150, 30);
	deleteButton->setEnabled(false);
	connect (deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelectedNeurons()));
	buttonLayout->addWidget(deleteButton);
	buttonLayout->addStretch(10);
	verticalBox->addLayout(buttonLayout);

	//Construct table view and model
	neuronGroupModel = new NeuronGroupModel();
	QTableView* neuronGroupView = new NeuronGroupTableView(this, neuronGroupModel);
	verticalBox->addWidget(neuronGroupView);

	//Enable or disable buttons depending on whether a network is loaded or not
	connect(Globals::getEventRouter(), SIGNAL(networkChangedSignal()), this, SLOT(networkChanged()));
}


/*! Destructor */
NeuronGroupWidget::~NeuronGroupWidget(){
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Shows plugins dialog configured to load up available plugins for adding neurons */
void NeuronGroupWidget::addNeurons(){
	//Run some checks - cannot change network if simulation or analysis is going on.
	if(!Globals::networkChangeOk())
		return;
	PluginsDialog* pluginsDialog = new PluginsDialog(this, "/plugins/neurons", "Add Neurons");
	pluginsDialog->exec();
	delete pluginsDialog;
}


/*! Deletes selected neuron groups */
void NeuronGroupWidget::deleteSelectedNeurons(){
	//Run some checks - cannot change network if simulation or analysis is going on.
	if(!Globals::networkLoaded()){
		qCritical()<<"Attempting to delete neurons when network is not loaded";
		return;
	}
	//Check if network is editable or not
	if(Globals::getNetwork()->hasArchives()){
		qWarning()<<"You cannot alter a network that has archives.\nDelete the archives and try again.";
		return;
	}
	if(!Globals::networkChangeOk())
		return;

	//Do nothing if no neuron groups are selected
	QList<unsigned int> deleteNeurIDList = neuronGroupModel->getSelectedNeuronGroupIDs();
	if(deleteNeurIDList.isEmpty())
		return;

	//Show warning
	QMessageBox msgBox(QMessageBox::Warning, "Deleting Neuron Groups", "Are you sure that you want to delete these neuron groups?\nThis step cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel, this);
	if(msgBox.exec() != QMessageBox::Ok)
		return;

	//Delete neuron groups
	try{
		connect(Globals::getNetwork(), SIGNAL(taskFinished()), this, SLOT(networkTaskFinished()),  Qt::UniqueConnection);
		progressDialog = new QProgressDialog("Deleting neuron group(s)", "Cancel", 0, 0, this);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->show();
		Globals::getNetwork()->deleteNeuronGroups(deleteNeurIDList);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();

		//Call networkTaskFinished to handle error
		networkTaskFinished();
	}
}


/*! Enables or disables buttons depending on whether a network is loaded */
void NeuronGroupWidget::networkChanged(){
	if(Globals::networkLoaded()){
		addNeuronsButton->setEnabled(true);
		deleteButton->setEnabled(true);
	}
	else{
		addNeuronsButton->setEnabled(false);
		deleteButton->setEnabled(false);
	}
}


/*! Called when the network has finished deleting neurons.
	Informs other classes that network has changed. */
void NeuronGroupWidget::networkTaskFinished(){
	//Check for errors
	if(Globals::getNetwork()->isError())
		qCritical()<<Globals::getNetwork()->getErrorMessage();
	Globals::getNetwork()->clearError();

	progressDialog->setValue(100);
	progressDialog->close();
	//delete progressDialog;

	//Clear selection on neuron group model
	neuronGroupModel->clearSelection();

	//Prevent this method being called when network finishes other tasks
	disconnect(Globals::getNetwork(), SIGNAL(taskFinished()), this, SLOT(networkTaskFinished()));

	//Inform other classes that network has changed
	Globals::getEventRouter()->networkChangedSlot();
}

