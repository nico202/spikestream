#include "TruthTableView.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
TruthTableView::TruthTableView(TruthTableModel* model) : QTableView() {
    //Set up the view
    setShowGrid(false);
    setSelectionMode(QAbstractItemView::NoSelection);

    //Set the model for this view and show it
    setModel(model);
    show();
    resizeHeaders();
}


/*! Destructor */
TruthTableView::~TruthTableView(){
}


/*! Sets up the size of the headers. */
void TruthTableView::resizeHeaders(){
    QHeaderView* hHeader = horizontalHeader();
    hHeader->setMinimumSectionSize(10);
    hHeader->setDefaultAlignment(Qt::AlignLeft);
}



