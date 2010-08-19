#ifndef VIEWPARAMETERSDIALOG_H
#define VIEWPARAMETERSDIALOG_H

//Qt includes
#include <QDialog>
#include <QHash>

namespace spikestream {

	/*! Displays supplied parametes in a non-editable format */
	class ViewParametersDialog : public QDialog {
		Q_OBJECT

		public:
			ViewParametersDialog(QHash<QString, double> parameterMap, QWidget* parent);
			~ViewParametersDialog();
	};

}

#endif//VIEWPARAMETERSDIALOG
