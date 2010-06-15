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
    truthTableModel = model;
    setModel(model);
    show();

    //Listen for resize events
    connect(model, SIGNAL(modelReset()), this, SLOT(resizeHeaders()));
}


/*! Destructor */
TruthTableView::~TruthTableView(){
}


/*-------------------------------------------------------------*/
/*-------                   PRIVATE SLOTS                ------*/
/*-------------------------------------------------------------*/

/*! Sets up the size of the headers. */
void TruthTableView::resizeHeaders(){
    QHeaderView* hHeader = horizontalHeader();

    //Resize columns
    if(truthTableModel->columnCount() > 0){
		for(int i=0; i<truthTableModel->columnCount()-1; ++i)
			hHeader->resizeSection(hHeader->logicalIndex(i), 40);//Neuron ID column
		hHeader->resizeSection(hHeader->logicalIndex(truthTableModel->columnCount()-1), 50);//Output column
    }

    hHeader->setDefaultAlignment(Qt::AlignLeft);
}



