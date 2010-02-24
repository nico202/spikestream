#ifndef ABSTRACTEXPORTANALYSISDIALOG_H
#define ABSTRACTEXPORTANALYSISDIALOG_H

//Qt includes
#include <QDialog>
#include <QLineEdit>
#include <QString>
#include <QComboBox>

namespace spikestream {

	class AbstractExportAnalysisDialog : public QDialog {
		Q_OBJECT

		public:
			AbstractExportAnalysisDialog(const QString& analysisName, QWidget* parent);
			virtual ~AbstractExportAnalysisDialog();


		private slots:
			void getFile();
			void okButtonClicked();

		protected:
			virtual void exportCommaSeparated(const QString& filePath) = 0;
			QString getAnalysisName() { return analysisName; }

		private:
			//======================  VARIABLES  =====================
			/*! The type of analysis being exported */
			QString analysisName;

			/*! Holds the file information */
			QLineEdit* fileLineEdit;

			/*! Holds the type of export */
			QComboBox* exportTypeCombo;


			//======================  METHODS  =======================
			QString getFilePath(QString fileFilter);
	};

}

#endif//ABSTRACTEXPORTANALYSISDIALOG_H
