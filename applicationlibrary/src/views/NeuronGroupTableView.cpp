//SpikeStream includes
#include "NeuronGroupDialog.h"
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
	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(tableDoubleClicked(QModelIndex)));

	//Header click events
	this->horizontalHeader()->setClickable(true);
	connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClicked(int)));

	//Set the model for this view and show it
	this->neuronGroupModel = model;
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
NeuronGroupTableView::~NeuronGroupTableView(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void NeuronGroupTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::VIS_COL), 20);//Visibility icon
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::ZOOM_COL), 20);//Zoom icon
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::ID_COL), 50);//ID
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::NAME_COL), 100);//Name
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::DESC_COL), 300);//Description
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::SIZE_COL), 100);//Size
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::NEUR_TYPE_COL), 200);//Neuron type
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::SELECT_COL), 20);//Selection
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupModel::PARAM_COL), 70);//Parameters
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Called when the header of the table is clicked */
void NeuronGroupTableView::headerClicked(int column){
	if(column == NeuronGroupModel::SELECT_COL)
		neuronGroupModel->selectAllOrNone();
	else if(column == NeuronGroupModel::VIS_COL)
		neuronGroupModel->showAllOrNone();
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


/*! Called when the table is double clicked.
	Displays a dialog to change name and description. */
void NeuronGroupTableView::tableDoubleClicked(QModelIndex index){
	if(index.column() == NeuronGroupModel::DESC_COL || index.column() == NeuronGroupModel::NAME_COL){
		try{
			NeuronGroupDialog* dialog = new NeuronGroupDialog(neuronGroupModel->getInfo(index), this);
			if(dialog->exec() == QDialog::Accepted){
				neuronGroupModel->reload();
			}
			delete dialog;
		}
		catch(SpikeStreamException& ex){
			qCritical()<<ex.getMessage();
		}
	}
}

