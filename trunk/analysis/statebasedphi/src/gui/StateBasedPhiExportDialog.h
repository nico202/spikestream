#ifndef STATEBASEDPHIEXPORTDIALOG_H
#define STATEBASEDPHIEXPORTDIALOG_H

//SpikeStream includes
#include "AbstractExportAnalysisDialog.h"


namespace spikestream {

	class StateBasedPhiExportDialog : public AbstractExportAnalysisDialog {
		Q_OBJECT

		public:
			StateBasedPhiExportDialog(QWidget* parent = 0);
			virtual ~StateBasedPhiExportDialog();


		protected:
			void exportCommaSeparated(const QString& filePath);
	};

}

#endif//STATEBASEDPHIEXPORTDIALOG_H

