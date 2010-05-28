//SpikeStream includes
#include "SpikeStreamException.h"
#include "SynapseParametersEditDialog.h"
#include "SynapseParametersView.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
SynapseParametersView::SynapseParametersView(QWidget* parent, SynapseParametersModel* model) : QTableView(parent) {
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
SynapseParametersView::~SynapseParametersView(){
}


/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void SynapseParametersView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(0), 100);//Description
	QList<ParameterInfo> conParams = model->getParameterInfoList();
	for(int i=0; i<conParams.size(); ++i){
		int tmpParamLength = 10+conParams.at(i).getName().length() * 10;
		if(tmpParamLength < 40)
			tmpParamLength = 40;
		hHeader->resizeSection(hHeader->logicalIndex(i+1), tmpParamLength);//Parameter name
	}

	//Icon
	hHeader->resizeSection(hHeader->logicalIndex(conParams.size() + 1), 50);//Edit button
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*! Called when the table is clicked and passes this information on to model. */
void SynapseParametersView::tableClicked(QModelIndex index){
	if(index.column() != model->getParameterInfoList().size() + 1)
		return;
	try{
		SynapseParametersEditDialog dialog(model->getConnectionGroupInfo(index.row()), model->getParameterInfoList(), model->getParameterValues(index.row()), this);
		if(dialog.exec() == QDialog::Accepted)
			model->reload();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}

