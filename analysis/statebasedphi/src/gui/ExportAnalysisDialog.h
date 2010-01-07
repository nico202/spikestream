#ifndef EXPORTANALYSISDIALOG_H
#define EXPORTANALYSISDIALOG_H

//Qt includes
#include <QDialog>
#include <QLineEdit>
#include <QString>
#include <QComboBox>

namespace spikestream {

    class ExportAnalysisDialog : public QDialog {
	Q_OBJECT

	public:
	    ExportAnalysisDialog(QWidget* parent);
	    ~ExportAnalysisDialog();


	private slots:
	    void getFile();
	    void okButtonClicked();


	private:
	    //======================  VARIABLES  =====================
	    /*! Holds the file information */
	    QLineEdit* fileLineEdit;

	    /*! Holds the type of export */
	    QComboBox* exportTypeCombo;


	    //======================  METHODS  =======================
	    void exportCommaSeparated(const QString& filePath);
	    QString getFilePath(QString fileFilter);
    };

}

#endif//EXPORTANALYSISDIALOG_H
