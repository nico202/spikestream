#ifndef NEURONPARAMETERSVIEW_H
#define NEURONPARAMETERSVIEW_H

//SpikeStream includes
#include "NeuronParametersModel.h"

//Qt includes
#include <QTableView>


namespace spikestream {

	/*! A table of neuron parameters for the current network */
	class NeuronParametersView : public QTableView {
		Q_OBJECT

		public:
			NeuronParametersView(QWidget* parent, NeuronParametersModel* model);
			~NeuronParametersView();

		private slots:
			void resizeHeaders();
			void tableClicked(QModelIndex index);

		private:
			//======================  VARIABLES  =====================
			/*! The model associated with the view */
			NeuronParametersModel* model;
	};

}

#endif//NEURONPARAMETERSVIEW_H

