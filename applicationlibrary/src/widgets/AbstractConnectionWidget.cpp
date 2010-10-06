//SpikeStream includes
#include "AbstractConnectionWidget.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "SynapseType.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QMessageBox>


/*! Constructor */
AbstractConnectionWidget::AbstractConnectionWidget(QWidget *parent) : QWidget(parent){

}


/*! Destructor */
AbstractConnectionWidget::~AbstractConnectionWidget(){
	delete connectionBuilder;
}


/*----------------------------------------------------------*/
/*-----               PROTECTED METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds the neuron groups from the current network to the combo box */
void AbstractConnectionWidget::addNeuronGroups(QComboBox* combo){
	QList<NeuronGroupInfo> neurGrpInfoList = Globals::getNetwork()->getNeuronGroupsInfo();
	foreach(NeuronGroupInfo neurGrpInfo, neurGrpInfoList){
		combo->addItem(neurGrpInfo.getName() + " (" + QString::number(neurGrpInfo.getID()) + ")");
	}
}


/*! Adds synapse types to the specified combo */
void AbstractConnectionWidget::addSynapseTypes(QComboBox* combo){
	QList<SynapseType> synapseTypesList = Globals::getNetworkDao()->getSynapseTypes();
	foreach(SynapseType synType, synapseTypesList){
		combo->addItem(synType.getDescription() + " (" + QString::number(synType.getID()) + ")");
	}
}


/*! Checks that a combo box has at least one entry */
void AbstractConnectionWidget::checkInput(QComboBox* combo, const QString& errorMessage){
	if(combo->count() == 0){
		throw SpikeStreamException(errorMessage);
	}
}


/*! Checks that a line edit has a valid input */
void AbstractConnectionWidget::checkInput(QLineEdit* inputEdit, const QString& errMsg){
	if(inputEdit->text().isEmpty()){
		throw SpikeStreamException(errMsg);
	}
}


/*! Extracts the neuron group ID from the text of a combo box */
unsigned int AbstractConnectionWidget::getNeuronGroupID(const QString& comboText){
	if(comboText.isEmpty())
		throw SpikeStreamException("Cannot extract a neuron group ID from empty text.");

	QRegExp regExp("[()]");
	return Util::getUInt(comboText.section(regExp, 1, 1));
}


/*! Extracts the synapse type id from the text of a combo box */
unsigned int AbstractConnectionWidget::getSynapseTypeID(const QString& comboText){
	if(comboText.isEmpty())
		throw SpikeStreamException("Cannot extract a synapse type ID from empty text.");

	QRegExp regExp("[()]");
	return Util::getUInt(comboText.section(regExp, 1, 1));
}



/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Called when add button is clicked.
	Checks the input and adds the specified network to the database */
void AbstractConnectionWidget::addButtonClicked(){
	//Double check network is loaded
	if(!Globals::networkLoaded()){
		QMessageBox::critical(this, "Random1 Connection Group Builder Error", "No network loaded.", QMessageBox::Ok);
		return;
	}

	//Check inputs have sensible values
	if(!checkInputs())
		return;

	try{
		//Get connection group info
		ConnectionGroupInfo info = getConnectionGroupInfo();

		//Start thread to add connections
		connectionBuilder->startBuildConnectionGroup(info);
		progressDialog = new QProgressDialog("Adding connection group", "Cancel", 0, 100, this, Qt::CustomizeWindowHint);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(2000);
		progressDialog->setCancelButton(0);//Too complicated to implement cancel sensibly
		updatingProgress = false;
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}

}


/*! Called when the builder thread has finished */
void AbstractConnectionWidget::builderThreadFinished(){
	if(connectionBuilder->isError())
		qCritical()<<connectionBuilder->getErrorMessage();

	//Close progress dialog
	progressDialog->close();

	//Inform other classes that network has changed
	Globals::getEventRouter()->networkChangedSlot();
}


/*! Updates user with feedback about progress with the operation */
void AbstractConnectionWidget::updateProgress(int stepsCompleted, int totalSteps, QString message){
	//Set flag to avoid multiple calls to progress dialog while it is redrawing
	if(updatingProgress)
		return;
	updatingProgress = true;

	//Check for cancellation
	if(progressDialog->wasCanceled()){
		qCritical()<<"This plugin does not currently support cancellation of adding connections.";
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

