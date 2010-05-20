//SpikeStream includes
#include "NeuronParametersEditDialog.h"
#include "NeuronParametersView.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
NeuronParametersView::NeuronParametersView(QWidget* parent, NeuronParametersModel* model) : QTableView(parent) {
	this->model = model;

	//Set up the view
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));

	//Set the model for this view and show it
	setModel(model);
	show();
	resizeHeaders();

	//Listen for resize events
	connect(model, SIGNAL(modelReset()), this, SLOT(resizeHeaders()));
}


/*! Destructor */
NeuronParametersView::~NeuronParametersView(){
}


/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void NeuronParametersView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(0), 100);//Description
	QList<ParameterInfo> neuronParams = model->getParameterInfoList();
	for(int i=0; i<neuronParams.size(); ++i){
		int tmpParamLength = 10+neuronParams.at(i).getName().length() * 10;
		if(tmpParamLength < 40)
			tmpParamLength = 40;
		hHeader->resizeSection(hHeader->logicalIndex(i+1), tmpParamLength);//Parameter name
	}

	//Icon
	hHeader->resizeSection(hHeader->logicalIndex(neuronParams.size() + 1), 50);//Edit button
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*! Called when the table is clicked and passes this information on to model. */
void NeuronParametersView::tableClicked(QModelIndex index){
	if(index.column() != model->getParameterInfoList().size() + 1)
		return;

	NeuronParametersEditDialog dialog(model->getNeuronGroupInfo(index.row()), model->getParameterInfoList(), model->getParameterValues(index.row()), this);
	if(dialog.exec() == QDialog::Accepted)
		model->reload();
}

