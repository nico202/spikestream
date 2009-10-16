#ifndef STATEBASEDPHIPARAMETERDIALOG_H
#define STATEBASEDPHIPARAMETERDIALOG_H

//SpikeStream includes
#include "AnalysisInfo.h"

//Qt includes
#include <QDialog>
#include <QLineEdit>
#include <QHash>
#include <QString>

namespace spikestream {

    class StateBasedPhiParameterDialog : public QDialog {
	Q_OBJECT

	public:
	    StateBasedPhiParameterDialog(QWidget* parent, const AnalysisInfo& info);
	    ~StateBasedPhiParameterDialog();

	    const AnalysisInfo& getInfo();

	private slots:
	    void okButtonClicked();

	private:
	    //=======================  VARIABLES  =========================
	    AnalysisInfo info;

	    QLineEdit* descriptionEdit;

	    QLineEdit* numThreadsEdit;

	    QHash<QString, QLineEdit*> editMap;

    };

}

#endif//STATEBASEDPHIPARAMETERDIALOG_H
