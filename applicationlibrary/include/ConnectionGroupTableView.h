#ifndef CONNECTIONGROUPTABLEVIEW_H
#define CONNECTIONGROUPTABLEVIEW_H

//SpikeStream includes
#include "ConnectionGroupModel.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! A table view of the connection group information. */
	class ConnectionGroupTableView : public QTableView {
		Q_OBJECT

		public:
			ConnectionGroupTableView(QWidget* parent, ConnectionGroupModel* model);
			~ConnectionGroupTableView();
			void resizeHeaders();

		private slots:
			void tableClicked(QModelIndex index);
			void tableDoubleClicked(QModelIndex index);

		private:
			//======================  VARIABLES  ======================
			/*! Model associated with this view */
			ConnectionGroupModel* connectionGroupModel;

	};

}

#endif//CONNECTIONGROUPTABLEVIEW_H
