#ifndef ANALYSESTABLEVIEW_H
#define ANALYSESTABLEVIEW_H

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! View of model listing the different types of analysis. Used for loading and deleting analyses. */
    class AnalysesTableView : public QTableView {
		Q_OBJECT

	public:
		AnalysesTableView(QWidget* parent, QAbstractTableModel* model);
	    ~AnalysesTableView();
	    void resizeHeaders();

	private slots:
	    void tableClicked(QModelIndex index);

    };

}

#endif//ANALYSESTABLEVIEW_H
