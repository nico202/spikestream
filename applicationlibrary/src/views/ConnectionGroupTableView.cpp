//SpikeStream includes
#include "ConnectionGroupTableView.h"
#include "SpikeStreamException.h"
#include "ViewParametersDialog.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
ConnectionGroupTableView::ConnectionGroupTableView(QWidget* parent, ConnectionGroupModel* model) : QTableView(parent) {
	//Set up the view
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));

	//Set the model for this view and show it
	this->connectionGroupModel = model;
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
ConnectionGroupTableView::~ConnectionGroupTableView(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void ConnectionGroupTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::VIS_COL), 20);//Visibility icon
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::ID_COL), 50);//ID
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::DESC_COL), 300);//Description
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::SIZE_COL), 100);//Size
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::FROM_NEUR_ID_COL), 100);//From neuron group
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::T0_NEUR_ID_COL), 100);//To neuron group
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::SYNAPSE_TYPE_COL), 100);//Synapse type
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::SELECT_COL), 20);//Select
	hHeader->resizeSection(hHeader->logicalIndex(ConnectionGroupModel::PARAM_COL), 70);//Parameters
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Called when the table is clicked and passes this information on to model. */
void ConnectionGroupTableView::tableClicked(QModelIndex index){
	if(index.column() == ConnectionGroupModel::PARAM_COL){
		try{
			ViewParametersDialog dlg(connectionGroupModel->getParameters(index.row()), this);
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

