//SpikeStream includes
#include "ChannelTableView.h"
#include "EditPropertiesDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>

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
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::CHANNEL_NAME_COL), 200);//Name of channel
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::NEURON_GROUP_NAME_COL), 200);//Name of neuron group
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::PARAM_COL), 20);//Channel parameters
	hHeader->resizeSection(hHeader->logicalIndex(ChannelModel::DELETE_COL), 20);//Delete button
	hHeader->setDefaultAlignment(Qt::AlignLeft);
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Called when the table is clicked and passes this information on to model. */
void ChannelTableView::tableClicked(QModelIndex index){
	try{
		if(index.column() == ChannelModel::PARAM_COL){
			map<string, Property*> tmpParamMap = channelModel->getParameters(index.row());
			EditPropertiesDialog* tmpDlg = new EditPropertiesDialog(tmpParamMap);
			tmpDlg->exec();
			return;
		}
		else if(index.column() == ChannelModel::DELETE_COL){
			//Check if user really wants to delete a channel
			int response = QMessageBox::warning(this, "Delete Channel?", "Are you sure that you want to delete this channel?\nThis cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
			if(response != QMessageBox::Ok)
				return;

			//Delete channel
			channelModel->deleteChannel(index.row());
			return;
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred configuring or deleting a channel.";
	}
}



