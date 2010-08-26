#ifndef PARAMETERSDIALOG_H
#define PARAMETERSDIALOG_H

//SpikeStream includes
#include "AbstractParametersEditDialog.h"

namespace spikestream {

	/*! Edits the supplied parameters  */
	class ParametersDialog : public AbstractParametersEditDialog {
		Q_OBJECT

		public:
			ParametersDialog(const QList<ParameterInfo>& paramInfoList, const QHash<QString, double>& defaultParameterMap, const QHash<QString, double>& currentParameterMap, QWidget* parent=0);
			~ParametersDialog();
			QHash<QString, double> getParameters() { return currentParameterMap; }


		private slots:
			void defaultButtonClicked();
			void okButtonClicked();

		private:
			//=====================  VARIABLES  ======================
			/*! Map containing default values for parameters */
			QHash<QString, double> defaultParameterMap;

			/*! Map containing current values of parameters */
			QHash<QString, double> currentParameterMap;
	};

}

#endif//PARAMETERSDIALOG_H
