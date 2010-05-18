#ifndef NEURONPARAMETERSVIEW_H
#define NEURONPARAMETERSVIEW_H

//Qt includes
#include <QTableView>


namespace spikestream {

	/*! A table of neuron parameters for the current network */
	class NeuronParametersView : public QTableView {
		Q_OBJECT

		public:
			NeuronParametersView(QWidget* parent, QAbstractTableModel* model);
			~NeuronParametersView();
			void resizeHeaders();

		private slots:
			void tableClicked(QModelIndex index);

	};

}

#endif//NEURONPARAMETERSVIEW_H

