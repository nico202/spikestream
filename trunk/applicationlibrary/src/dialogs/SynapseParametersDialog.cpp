#include "ConnectionGroupInfo.h"
#include "Globals.h"
#include "SynapseParametersDialog.h"
#include "SynapseParametersModel.h"
#include "SynapseParametersView.h"
using namespace spikestream;

/*! Constructor */
SynapseParametersDialog::SynapseParametersDialog(QWidget *parent) : QDialog(parent) {
	//Set title
	this->setWindowTitle("Synapse Parameters");

	//Get a list of the connection groups in the network
	QList<ConnectionGroupInfo> infoList = Globals::getNetwork()->getConnectionGroupsInfo();

	//Create a tab for each synapse type in the network
	QTabWidget* tabWidget = new QTabWidget(this);
	QHash<unsigned int, bool> synTypeMap;
	foreach(ConnectionGroupInfo conGrpInfo, infoList){
		unsigned int tmpSynTypeID = conGrpInfo.getSynapseTypeID();
		if(!synTypeMap.contains(tmpSynTypeID)){
			SynapseParametersModel* synapseParametersModel = new SynapseParametersModel(tmpSynTypeID);
			QTableView* synapseParametersView = new SynapseParametersView(tabWidget, synapseParametersModel);
			SynapseType synType = Globals::getNetworkDao()->getSynapseType(tmpSynTypeID);
			tabWidget->addTab(synapseParametersView, synType.getDescription());
			synTypeMap[tmpSynTypeID] = true;
		}
	}

	tabWidget->setMinimumSize(600, 400);
}



/*! Destructor */
SynapseParametersDialog::~SynapseParametersDialog(){
}

