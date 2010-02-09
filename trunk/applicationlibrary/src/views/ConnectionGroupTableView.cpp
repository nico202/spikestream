#include "ConnectionGroupTableView.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
ConnectionGroupTableView::ConnectionGroupTableView(QWidget* parent, QAbstractTableModel* model) : QTableView(parent) {
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
ConnectionGroupTableView::~ConnectionGroupTableView(){
}


/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void ConnectionGroupTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(0), 20);//Visibility icon
	hHeader->resizeSection(hHeader->logicalIndex(1), 50);//ID
	hHeader->resizeSection(hHeader->logicalIndex(2), 300);//Description
	hHeader->resizeSection(hHeader->logicalIndex(3), 100);//From neuron group
	hHeader->resizeSection(hHeader->logicalIndex(4), 100);//To neuron group
	hHeader->resizeSection(hHeader->logicalIndex(5), 100);//Synapse type
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*! Called when the table is clicked and passes this information on to model. */
void ConnectionGroupTableView::tableClicked(QModelIndex index){
	if(index.column() == 0)
	model()->setData(index, 0);
}

