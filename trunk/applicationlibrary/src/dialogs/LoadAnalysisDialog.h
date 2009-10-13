#ifndef LOADANALYSISDIALOG_H
#define LOADANALYSISDIALOG_H

//SpikeStream includes
#include "AnalysisInfo.h"
using namespace spikestream;

//Qt includes
#include <QDialog>
#include <QTableView>
#include <QItemSelectionModel>
#include <QSqlQueryModel>

namespace spikestream {

    class LoadAnalysisDialog : public QDialog {
	Q_OBJECT

	public:
	    LoadAnalysisDialog(QWidget* parent);
	    ~LoadAnalysisDialog();
	   const AnalysisInfo& getAnalysisInfo();

	private slots:
	    void okButtonPressed();
	    void selectionChanged( const QModelIndex & current, const QModelIndex & previous);

	private:
	    //====================  VARIABLES  =======================
	    /*! The table displaying the analyses available for this network */
	     QTableView* analysisTableView;

	     QItemSelectionModel *selectionModel;

	     QSqlQueryModel* model;

	     /*! Holds information about the currently loaded analysis if there is one. */
	     AnalysisInfo analysisInfo;
    };

}

#endif//LOADANALYSISDIALOG_H

