#include "ConnectionsTableView.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>


/*! Constructor */
ConnectionsTableView::ConnectionsTableView(QWidget* parent, QAbstractTableModel* model) : QTableView(parent) {
	//Set up the view
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);

	//Set the model for this view and show it
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
ConnectionsTableView::~ConnectionsTableView(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void ConnectionsTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(0), 100);//ID
	hHeader->resizeSection(hHeader->logicalIndex(1), 50);//From ID
	hHeader->resizeSection(hHeader->logicalIndex(2), 50);//To ID
	hHeader->resizeSection(hHeader->logicalIndex(3), 50);//Delay
	hHeader->resizeSection(hHeader->logicalIndex(4), 50);//Weight
	hHeader->resizeSection(hHeader->logicalIndex(5), 100);//Temporary weight
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


