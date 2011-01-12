//SpikeStream includes
#include "NeuronGroupSelectionView.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
NeuronGroupSelectionView::NeuronGroupSelectionView(QWidget* parent, NeuronGroupSelectionModel* model) : QTableView(parent) {
	//Set up the view
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));

	//Header click events
	this->horizontalHeader()->setClickable(true);
	connect(this->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClicked(int)));

	//Set the model for this view and show it
	this->neuronGroupSelectionModel = model;
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
NeuronGroupSelectionView::~NeuronGroupSelectionView(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void NeuronGroupSelectionView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupSelectionModel::SELECT_COL), 50);
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupSelectionModel::NAME_COL), 200);
	hHeader->resizeSection(hHeader->logicalIndex(NeuronGroupSelectionModel::ID_COL), 50);
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Called when the header of the table is clicked */
void NeuronGroupSelectionView::headerClicked(int column){
	if(column == NeuronGroupSelectionModel::SELECT_COL)
		neuronGroupSelectionModel->selectAllOrNone();
}


/*! Called when the table is clicked and passes this information on to model. */
void NeuronGroupSelectionView::tableClicked(QModelIndex index){
	model()->setData(index, 0);
}




