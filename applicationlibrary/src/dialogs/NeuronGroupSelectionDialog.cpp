//SpikeStream includes
#include "NeuronGroupSelectionDialog.h"
using namespace spikestream;

//SpikeStream includes
#include "NeuronGroupSelectionModel.h"
#include "NeuronGroupSelectionView.h"

//Qt includes
#include <QLayout>
#include <QPushButton>


/*! Constructor */
NeuronGroupSelectionDialog::NeuronGroupSelectionDialog(Network* network, QWidget* parent) : QDialog(parent){
	//Set title
	this->setWindowTitle("Select Neuron Groups");

	//Vertical box to organize layout
	QVBoxLayout* verticalBox = new QVBoxLayout(this);

	//Add table view and model
	neurGrpSelectionModel = new NeuronGroupSelectionModel(network->getNeuronGroups());
	QTableView* neuronGroupSelectionView = new NeuronGroupSelectionView(this, neurGrpSelectionModel);
	verticalBox->addWidget(neuronGroupSelectionView);

	//Ok and cancel buttons
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	QPushButton* cancelButton = new QPushButton("Cancel");
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	buttonLayout->addWidget(cancelButton);
	QPushButton* okButton = new QPushButton("Ok");
	connect (okButton, SIGNAL(clicked()), this, SLOT(accept()));
	buttonLayout->addWidget(okButton);
	verticalBox->addLayout(buttonLayout);

	this->setMinimumSize(400, 800);
}


/*! Destructor */
NeuronGroupSelectionDialog::~NeuronGroupSelectionDialog(){
}


/*! Returns a list of neuron groups that were selected by the user. */
QList<NeuronGroup*> NeuronGroupSelectionDialog::getNeuronGroups(){
	return neurGrpSelectionModel->getSelectedNeuronGroups();
}
