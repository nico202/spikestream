#include "NeuronGroupTableView.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
NeuronGroupTableView::NeuronGroupTableView(QWidget* parent, QAbstractTableModel* model) : QTableView(parent) {
	//Set up the view
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));

	//Set the model for this view and show it
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
	hHeader->resizeSection(hHeader->logicalIndex(0), 20);//Visibility icon
	hHeader->resizeSection(hHeader->logicalIndex(1), 20);//Zoom icon
	hHeader->resizeSection(hHeader->logicalIndex(2), 50);//ID
	hHeader->resizeSection(hHeader->logicalIndex(3), 200);//Name
	hHeader->resizeSection(hHeader->logicalIndex(4), 300);//Description
	hHeader->resizeSection(hHeader->logicalIndex(5), 100);//Neuron type
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*! Called when the table is clicked and passes this information on to model. */
void NeuronGroupTableView::tableClicked(QModelIndex index){
	if(index.column() == 0 || index.column() == 1 || index.column() == 6)
		model()->setData(index, 0);
}

