#ifndef NEURONPARAMETERSDIALOG_H
#define NEURONPARAMETERSDIALOG_H

//Qt includes
#include <QDialog>

namespace spikestream {

	/*! Displays the current neuron parameters for the currently loaded network.
		Clicking on a particular row launches a dialog to edit parameters for the corresponding neuron group
		NOTE: Not thread safe. */
	class NeuronParametersDialog : public QDialog {
		Q_OBJECT

		public:
			NeuronParametersDialog(QWidget* parent=0);
			~NeuronParametersDialog();

	};

}

#endif//NEURONPARAMETERSDIALOG_H

