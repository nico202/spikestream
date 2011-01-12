#ifndef NEURONGROUPSELECTIONVIEW_H
#define NEURONGROUPSELECTIONVIEW_H

//SpikeStream includes
#include "NeuronGroupSelectionModel.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! A view of the NeuronGroupSelectionModel displaying a selectable list of neuron groups. */
	class NeuronGroupSelectionView : public QTableView {
		Q_OBJECT

		public:
			NeuronGroupSelectionView(QWidget* parent, NeuronGroupSelectionModel* model);
			~NeuronGroupSelectionView();
			void resizeHeaders();

		private slots:
			void headerClicked(int column);
			void tableClicked(QModelIndex index);

		private:
			//====================  VARIABLES  =====================
			/*! Model associated with this view */
			NeuronGroupSelectionModel* neuronGroupSelectionModel;

	};

}

#endif//NEURONGROUPSELECTIONVIEW_H
