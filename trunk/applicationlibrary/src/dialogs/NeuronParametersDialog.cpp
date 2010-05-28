//SpikeStream includes
#include "Globals.h"
#include "NeuronParametersDialog.h"
#include "NeuronParametersModel.h"
#include "NeuronParametersView.h"
using namespace spikestream;

//Qt includes
#include <QTabWidget>


/*! Constructor */
NeuronParametersDialog::NeuronParametersDialog(QWidget *parent) : QDialog(parent) {
	//Set title
	this->setWindowTitle("Neuron Parameters");

	//Get a list of the neuron groups in the network
	QList<NeuronGroupInfo> infoList = Globals::getNetwork()->getNeuronGroupsInfo();

	//Create a tab for each neuron type in the network
	QTabWidget* tabWidget = new QTabWidget(this);
	QHash<unsigned int, bool> neurTypeMap;
	foreach(NeuronGroupInfo neurGrpInfo, infoList){
		unsigned int tmpNeurTypeID = neurGrpInfo.getNeuronTypeID();
		if(!neurTypeMap.contains(tmpNeurTypeID)){
			NeuronParametersModel* neuronParametersModel = new NeuronParametersModel(tmpNeurTypeID);
			QTableView* neuronParametersView = new NeuronParametersView(tabWidget, neuronParametersModel);
			NeuronType neurType = Globals::getNetworkDao()->getNeuronType(tmpNeurTypeID);
			tabWidget->addTab(neuronParametersView, neurType.getDescription());
			neurTypeMap[tmpNeurTypeID] = true;
		}
	}

	tabWidget->setMinimumSize(600, 400);
}


/*! Destructor */
NeuronParametersDialog::~NeuronParametersDialog(){
}
