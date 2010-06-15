#ifndef LOADANALYSISDIALOG_H
#define LOADANALYSISDIALOG_H

//SpikeStream includes
#include "AnalysisInfo.h"
#include "AnalysesModel.h"
#include "AnalysesTableView.h"
using namespace spikestream;

//Qt includes
#include <QDialog>
#include <QTableView>
#include <QItemSelectionModel>
#include <QSqlQueryModel>

namespace spikestream {

	/*! Dialog that allows the user to load and delete analyses. */
    class LoadAnalysisDialog : public QDialog {
		Q_OBJECT

		public:
			LoadAnalysisDialog(QWidget* parent, unsigned int analysisType);
			~LoadAnalysisDialog();
		   const AnalysisInfo& getAnalysisInfo();

		private slots:
		   void deleteButtonPressed();
			void okButtonPressed();

		private:
			//====================  VARIABLES  =======================
			 /*! Holds information about the currently selected analysis if there is one.
				If dialog is accepted the calling class should request this with getAnalysisInfo(). */
			 AnalysisInfo analysisInfo;

			 /*! Type of analysis being displayed by the dialog */
			 unsigned int analysisType;

			 /*! Model */
			 AnalysesModel* analysesModel;
    };

}

#endif//LOADANALYSISDIALOG_H

