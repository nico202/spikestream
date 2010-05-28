#ifndef ABSTRACTPARAMETERSEDITDIALOG_H
#define ABSTRACTPARAMETERSEDITDIALOG_H

//SpikeStream includes
#include "ParameterInfo.h"

//Qt includes
#include <QCheckBox>
#include <QDialog>
#include <QHash>
#include <QLayout>
#include <QLineEdit>

namespace spikestream {

	/*! Abstract class containing functions common to all dialogs for the editing of parameters */
	class AbstractParametersEditDialog : public QDialog {
		Q_OBJECT

		public:
			AbstractParametersEditDialog(const QList<ParameterInfo>& paramInfoList, QWidget* parent = 0);
			virtual ~AbstractParametersEditDialog();


		protected slots:
			virtual void defaultButtonClicked() = 0;
			virtual void okButtonClicked() = 0;


		protected:
			//===================  VARIABLES  ====================
			/*! Information about the parameters that are being edited */
			QList<ParameterInfo> parameterInfoList;

			//====================  METHODS  =====================
			void addButtons(QVBoxLayout* mainVLayout);
			void addParameters(QVBoxLayout* mainVLayout);
			QHash<QString, double> getParameterValues();
			void setParameterValues(const QHash<QString, double>& paramMap);

		private:
			//===================  VARIABLES  ====================
			/*! Map of the line edits for double parameters.
				Key is the name of the parameter; value is a pointer to the line edit */
			QHash<QString, QLineEdit*> lineEditMap;

			/*! Map of the check boxes for boolean parameters.
				Key is the name of the parameter; value is a pointer to the check box */
			QHash<QString, QCheckBox*> checkBoxMap;
	};

}

#endif//ABSTRACTPARAMETERSEDITDIALOG_H
