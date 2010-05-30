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
			NemoParametersDialog(const QList<ParameterInfo>& paramInfoList, const QHash<QString, double>& currParamValueMap, const QHash<QString, double>& defaultParamValueMap, QWidget* parent=0);
			~NemoParametersDialog();
			QHash<QString, double> getParameters(){ return parameterMap; }


		private slots:
			void defaultButtonClicked();
			void okButtonClicked();


		private:
			//=====================  VARIABLES  ======================
			/*! Map of the parameter values - only populated after the ok button has been
				clicked and the dialog closed */
			QHash<QString, double> parameterMap;

			/*! Map storing the default parameter values */
			QHash<QString, double> defaultParameterMap;


	};

}

#endif//NEMOPARAMETERSDIALOG_H

