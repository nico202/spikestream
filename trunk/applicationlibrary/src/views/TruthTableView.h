#ifndef TRUTHTABLEVIEW_H
#define TRUTHTABLEVIEW_H

//SpikeStream includes
#include "TruthTableModel.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

    class TruthTableView : public QTableView {
	Q_OBJECT

	public:
	    TruthTableView(TruthTableModel* model);
	    ~TruthTableView();
	    void resizeHeaders();

	private slots:

    };

}

#endif//TRUTHTABLEVIEW_H

