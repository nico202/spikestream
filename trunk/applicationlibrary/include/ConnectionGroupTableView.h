#ifndef CONNECTIONGROUPTABLEVIEW_H
#define CONNECTIONGROUPTABLEVIEW_H

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! A table view of the connection group information. */
	class ConnectionGroupTableView : public QTableView {
		Q_OBJECT

		public:
			ConnectionGroupTableView(QWidget* parent, QAbstractTableModel* model);
			~ConnectionGroupTableView();
			void resizeHeaders();

		private slots:
			void tableClicked(QModelIndex index);

	};

}

#endif//CONNECTIONGROUPTABLEVIEW_H
