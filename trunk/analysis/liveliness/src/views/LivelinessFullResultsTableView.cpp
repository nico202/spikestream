#include "LivelinessFullResultsTableView.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
LivelinessFullResultsTableView::LivelinessFullResultsTableView(QAbstractTableModel* model) : QTableView() {
	//Set up the view
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);

	//Handle user click events
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));

	//Set the model for this view and show it
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
LivelinessFullResultsTableView::~LivelinessFullResultsTableView(){
}


/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void LivelinessFullResultsTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(0), 50);//ID
	hHeader->resizeSection(hHeader->logicalIndex(1), 130);//Time Step
	hHeader->resizeSection(hHeader->logicalIndex(2), 50);//Liveliness
	hHeader->resizeSection(hHeader->logicalIndex(3), 200);//Neuron IDs
	hHeader->resizeSection(hHeader->logicalIndex(4), 20);//View
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*! Called when the table is clicked and passes this information on to model. */
void LivelinessFullResultsTableView::tableClicked(QModelIndex index){
	if(index.column() == viewCol)
	model()->setData(index, 0);
}


