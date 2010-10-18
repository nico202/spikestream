#ifndef NEURONGROUPTABLEVIEW_H
#define NEURONGROUPTABLEVIEW_H

//SpikeStream includes
#include "NeuronGroupModel.h"

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! A view of the NeuronGroupModel displaying information about the
		neuron groups in the current network. */
	class NeuronGroupTableView : public QTableView {
		Q_OBJECT

		public:
			NeuronGroupTableView(QWidget* parent, NeuronGroupModel* model);
			~NeuronGroupTableView();
			void resizeHeaders();

		private slots:
			void tableClicked(QModelIndex index);
			void tableDoubleClicked(QModelIndex index);

		private:
			//====================  VARIABLES  =====================
			/*! Model associated with this view */
			NeuronGroupModel* neuronGroupModel;

	};

}

#endif//NEURONGROUPTABLEVIEW_H
