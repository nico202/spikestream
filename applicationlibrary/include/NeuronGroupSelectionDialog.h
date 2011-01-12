#ifndef NEURONGROUPSELECTIONDIALOG_H
#define NEURONGROUPSELECTIONDIALOG_H

//SpikeStream includes
#include "Network.h"
#include "NeuronGroupSelectionModel.h"

//Qt includes
#include <QDialog>

namespace spikestream {

	/*! Displays a checkable list of the neuron groups in the supplied network.
		Enables user to select all or a subset of these neuron groups. */
	class NeuronGroupSelectionDialog : public QDialog {
		Q_OBJECT

		public:
			NeuronGroupSelectionDialog(Network* network, QWidget* parent = 0);
			~NeuronGroupSelectionDialog();
			QList<NeuronGroup*> getNeuronGroups();

		private:
			//=================  VARIABLES  ===================
			NeuronGroupSelectionModel* neurGrpSelectionModel;

	};

}


#endif//NEURONGROUPSELECTIONDIALOG_H
