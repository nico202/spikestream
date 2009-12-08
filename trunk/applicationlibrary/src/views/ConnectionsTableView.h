#ifndef CONNECTIONSTABLEVIEW_H
#define CONNECTIONSTABLEVIEW_H

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

    class ConnectionsTableView : public QTableView {
	Q_OBJECT

	public:
	    ConnectionsTableView(QAbstractTableModel* model);
	    ~ConnectionsTableView();
	    void resizeHeaders();

	private slots:

    };

}

#endif//CONNECTIONSTABLEVIEW_H
