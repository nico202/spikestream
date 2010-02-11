#ifndef FULLRESULTSTABLEVIEW_H
#define FULLRESULTSTABLEVIEW_H

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	class FullResultsTableView : public QTableView {
	Q_OBJECT

	public:
		FullResultsTableView(QWidget* parent, QAbstractTableModel* model);
		~FullResultsTableView();
		void resizeHeaders();

	private slots:
		void tableClicked(QModelIndex index);

	private:
		//=====================  VARIABLES  ==========================
		static const int viewCol = 4;//FIXME: BETTER TO GET THIS FROM MODEL


	};

}

#endif//FULLRESULTSTABLEVIEW_H
