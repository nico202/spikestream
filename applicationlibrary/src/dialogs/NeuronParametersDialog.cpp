//SpikeStream includes
#include "NeuronParametersDialog.h"
#include "NeuronParametersModel.h"
#include "NeuronParametersView.h"
using namespace spikestream;

//Qt includes
#include <QLayout>


/*! Constructor */
NeuronParametersDialog::NeuronParametersDialog(QWidget *parent) : QDialog(parent) {
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	//Construct table view and model
	NeuronParametersModel* neuronParametersModel = new NeuronParametersModel();
	QTableView* neuronParametersView = new NeuronParametersView(this, neuronParametersModel);
	mainVBox->addWidget(neuronParametersView);

	setMinimumSize(600, 400);
}


/*! Destructor */
NeuronParametersDialog::~NeuronParametersDialog(){
}
