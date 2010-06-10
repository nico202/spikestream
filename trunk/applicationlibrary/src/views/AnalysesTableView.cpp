#include "AnalysesTableView.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
AnalysesTableView::AnalysesTableView(QWidget* parent, QAbstractTableModel* model) : QTableView(parent) {
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
AnalysesTableView::~AnalysesTableView(){
}


/*------------------------------------------------------------*/
/*------               PUBLIC METHODS                   ------*/
/*------------------------------------------------------------*/

/*! Sets up the size of the headers. Does not work in the constructor because there
    is not any data or model */
void AnalysesTableView::resizeHeaders(){
    QHeaderView* hHeader = horizontalHeader();
    hHeader->setMinimumSectionSize(10);
    hHeader->resizeSection(hHeader->logicalIndex(0), 30);//Check box
    hHeader->resizeSection(hHeader->logicalIndex(1), 50);//ID
    hHeader->resizeSection(hHeader->logicalIndex(2), 50);//Network id
    hHeader->resizeSection(hHeader->logicalIndex(3), 50);//Archive ID
    hHeader->resizeSection(hHeader->logicalIndex(4), 80);//Time
    hHeader->resizeSection(hHeader->logicalIndex(5), 200);//Description
    hHeader->resizeSection(hHeader->logicalIndex(6), 50);//Parameters
    hHeader->resizeSection(hHeader->logicalIndex(7), 50);//type
    hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*------------------------------------------------------------*/
/*------               PRIVATE SLOTS                    ------*/
/*------------------------------------------------------------*/

/*! Called when the table is clicked and passes this information on to model. */
void AnalysesTableView::tableClicked(QModelIndex index){
    if(index.column() == 0 || index.column() == 1)
	model()->setData(index, 0);
}

