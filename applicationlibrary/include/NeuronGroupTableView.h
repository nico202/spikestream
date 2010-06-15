#ifndef NEURONGROUPTABLEVIEW_H
#define NEURONGROUPTABLEVIEW_H

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

	/*! A view of the NeuronGroupModel displaying information about the
		neuron groups in the current network. */
	class NeuronGroupTableView : public QTableView {
		Q_OBJECT

		public:
			NeuronGroupTableView(QWidget* parent, QAbstractTableModel* model);
			~NeuronGroupTableView();
			void resizeHeaders();

		private slots:
			void tableClicked(QModelIndex index);

	};

}

#endif//NEURONGROUPTABLEVIEW_H
