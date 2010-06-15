#ifndef TRUTHTABLEDIALOG_H
#define TRUTHTABLEDIALOG_H

//SpikeStream includes
#include "TruthTableModel.h"
using namespace spikestream;

//Qt includes
#include <QDialog>

namespace spikestream {

	/*! Displays a weightless neuron's truth table.  */
	class TruthTableDialog : public QDialog {
		Q_OBJECT

		public:
			TruthTableDialog(QWidget* parent=0);
			~TruthTableDialog();
			void show(unsigned int neuronID);


		private slots:
			void cancelButtonPressed();
			void okButtonPressed();


		private:
			//========================  VARIABLES  ========================
			TruthTableModel* truthTableModel;

	};

}

#endif//TRUTHTABLEDIALOG_H

