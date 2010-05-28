#ifndef NEMOPARAMETERSDIALOG_H
#define NEMOPARAMETERSDIALOG_H

//SpikeStream includes
#include "AbstractParametersEditDialog.h"

//Qt includes
#include <QDialog>

namespace spikestream {

	/*! Displays the current  parameters for the Nemo CUDA simulator. */
	class NemoParametersDialog : public AbstractParametersEditDialog {
		Q_OBJECT

		public:
			NemoParametersDialog(QWidget* parent=0);
			~NemoParametersDialog();

	};

}

#endif//NEMOPARAMETERSDIALOG_H

