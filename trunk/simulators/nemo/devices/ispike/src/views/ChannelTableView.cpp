//SpikeStream includes
#include "ChannelTableView.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>

/*! Constructor */
ChannelTableView::ChannelTableView(ChannelModel* model, QWidget* parent) : QTableView(parent) {
	//Set up the view
	setShowGrid(false);
	setSelectionMode(QAbstractItemView::NoSelection);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(tableClicked(QModelIndex)));

	//Set the model for this view and show it
	this->channelModel = model;
	setModel(model);
	show();
	resizeHeaders();
}


/*! Destructor */
ChannelTableView::~ChannelTableView(){
}


/*--------------------------------------------------------*/
/*-------             PUBLIC METHODS               -------*/
/*--------------------------------------------------------*/

/*! Sets up the size of the headers. Does not work in the constructor because there
	is not any data or model */
void ChannelTableView::resizeHeaders(){
	QHeaderView* hHeader = horizontalHeader();
	hHeader->setMinimumSectionSize(10);
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::CHANNEL_NAME_COL), 150);//Name of channel
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::NEURON_GROUP_NAME_COL), 150);//Name of neuron group
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::PARAM_COL), 50);//Channel parameters
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::DELETE_COL), 50);//Delete button
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Called when the table is clicked and passes this information on to model. */
void ChannelTableView::tableClicked(QModelIndex index){
	if(index.column() == ChannelModel::PARAM_COL){
		try{
//			ViewParametersDialog dlg(neuronGroupModel->getParameters(index.row()), this);
//			dlg.exec();
		}
		catch(SpikeStreamException& ex){
			qCritical()<<ex.getMessage();
		}
		return;
	}
	else if(index.column() == ChannelModel::DELETE_COL){
		//Check if user really wants to delete a channel

		//Delete channel
	}
}



