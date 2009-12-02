#ifndef CONNECTIONGROUPTABLEVIEW_H
#define CONNECTIONGROUPTABLEVIEW_H

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

    class ConnectionGroupTableView : public QTableView {
	Q_OBJECT

	public:
	    ConnectionGroupTableView(QAbstractTableModel* model);
	    ~ConnectionGroupTableView();
	    void resizeHeaders();

	private slots:
	    void tableClicked(QModelIndex index);

    };

}

#endif//CONNECTIONGROUPTABLEVIEW_H
