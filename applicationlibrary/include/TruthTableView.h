#ifndef TRUTHTABLEVIEW_H
#define TRUTHTABLEVIEW_H

//SpikeStream includes
#include "TruthTableModel.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! Table view for the display of the truth table associated with a particular weightless neuron. */
    class TruthTableView : public QTableView {
		Q_OBJECT

		public:
			TruthTableView(TruthTableModel* model);
			~TruthTableView();

		private slots:
			void resizeHeaders();

		private:
			//=====================  VARIABLES  =======================
			TruthTableModel* truthTableModel;

    };

}

#endif//TRUTHTABLEVIEW_H

