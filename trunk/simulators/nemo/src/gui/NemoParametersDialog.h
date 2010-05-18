#ifndef NEMOPARAMETERSDIALOG_H
#define NEMOPARAMETERSDIALOG_H

//Qt includes
#include <QDialog>

namespace spikestream {

	/*! Displays the current  parameters for the Nemo CUDA simulator. */
	class NemoParametersDialog : public QDialog {
		Q_OBJECT

		public:
			NemoParametersDialog(QWidget* parent=0);
			~NemoParametersDialog();

	};

}

#endif//NEMOPARAMETERSDIALOG_H

