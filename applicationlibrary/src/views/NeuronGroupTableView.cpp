//SpikeStream includes
#include "NeuronGroupTableView.h"
#include "SpikeStreamException.h"
#include "ViewParametersDialog.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
NeuronGroupTableView::NeuronGroupTableView(QWidget* parent, NeuronGroupModel* model) : QTableView(parent) {
	//Set up the view
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));

	//Set the model for this view and show it
	this->neuronGroupModel = model;
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
NeuronGroupTableView::~NeuronGroupTableView(){
}


/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void NeuronGroupTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::VIS_COL), 20);//Visibility icon
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::ZOOM_COL), 20);//Zoom icon
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::ID_COL), 50);//ID
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::NAME_COL), 200);//Name
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::DESC_COL), 300);//Description
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::SIZE_COL), 100);//Size
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::NEUR_TYPE_COL), 100);//Neuron type
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::SELECT_COL), 20);//Selection
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::PARAM_COL), 70);//Parameters
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*! Called when the table is clicked and passes this information on to model. */
void NeuronGroupTableView::tableClicked(QModelIndex index){
	if(index.column() == NeuronGroupModel::PARAM_COL){
		try{
			ViewParametersDialog dlg(neuronGroupModel->getParameters(index.row()), this);
			dlg.exec();
		}
		catch(SpikeStreamException& ex){
			qCritical()<<ex.getMessage();
		}
		return;
	}
	else{
		model()->setData(index, 0);
	}
}

