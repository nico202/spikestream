#ifndef CHANNELTABLEVIEW_H
#define CHANNELTABLEVIEW_H

//SpikeStream includes
#include "ChannelModel.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! A view of the ChannelModel displaying information about the
		currently active channel. */
	class ChannelTableView : public QTableView {
		Q_OBJECT

		public:
			ChannelTableView(ChannelModel* model, QWidget* parent=0);
			~ChannelTableView();
			void resizeHeaders();

		private slots:
			void tableClicked(QModelIndex index);

		private:
			//====================  VARIABLES  =====================
			/*! Model associated with this view */
			ChannelModel* channelModel;

	};

}

#endif//CHANNELTABLEVIEW_H
