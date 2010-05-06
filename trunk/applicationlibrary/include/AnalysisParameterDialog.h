#ifndef ANALYSISPARAMETERDIALOG_H
#define ANALYSISPARAMETERDIALOG_H

//SpikeStream includes
#include "AnalysisInfo.h"

//Qt includes
#include <QDialog>
#include <QLineEdit>
#include <QHash>
#include <QString>

namespace spikestream {

	class AnalysisParameterDialog : public QDialog {
	Q_OBJECT

	public:
		AnalysisParameterDialog(QWidget* parent, const AnalysisInfo& info);
		~AnalysisParameterDialog();
		const AnalysisInfo& getInfo();

	private slots:
		void okButtonClicked();

	private:
		//=======================  VARIABLES  =========================
		/*! Information about the analysis, containing the parameter map */
		AnalysisInfo info;

		/*! Used to edit the description of the analysis */
		QLineEdit* descriptionEdit;

		/*! Used to edit the number of threads */
		QLineEdit* numThreadsEdit;

		/*! Map of line edits used to edit the parameters */
		QHash<QString, QLineEdit*> editMap;

	};

}

#endif//ANALYSISPARAMETERDIALOG_H
