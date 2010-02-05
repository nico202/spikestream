#ifndef LIVELINESSEXPORTDIALOG_H
#define LIVELINESSEXPORTDIALOG_H

//SpikeStream includes
#include "AbstractExportAnalysisDialog.h"


namespace spikestream {

	class LivelinessExportDialog : public AbstractExportAnalysisDialog {
		Q_OBJECT

		public:
			LivelinessExportDialog(QWidget* parent = 0);
			virtual ~LivelinessExportDialog();


		protected:
			void exportCommaSeparated(const QString& filePath);
	};

}

#endif//LIVELINESSEXPORTDIALOG_H

