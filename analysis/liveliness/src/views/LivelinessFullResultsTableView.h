#ifndef LIVELINESSFULLRESULTSTABLEVIEW_H
#define LIVELINESSFULLRESULTSTABLEVIEW_H

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	class LivelinessFullResultsTableView : public QTableView {
	Q_OBJECT

	public:
		LivelinessFullResultsTableView(QWidget* parent, QAbstractTableModel* model);
		~LivelinessFullResultsTableView();
		void resizeHeaders();

	private slots:
		void tableClicked(QModelIndex index);

	private:
		//=====================  VARIABLES  ==========================
		static const int viewCol = 4;//FIXME: BETTER TO GET THIS FROM MODEL


	};

}

#endif//LIVELINESSFULLRESULTSTABLEVIEW_H
