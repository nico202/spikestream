#ifndef NRMIMPORTDIALOG_H
#define NRMIMPORTDIALOG_H

//Qt includes
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QString>

class NRMImportDialog : public QDialog {
    Q_OBJECT

    public:
	NRMImportDialog(QWidget* parent);
	~NRMImportDialog();

    private slots:
	void getConfigFile();
	void getTrainingFile();
	void nextButtonPressed();

    private:
	//======================  VARIABLES  =======================
	/*! Allows user to enter the path to the configuration file */
	QLineEdit* configFilePath;

	/*! Allows user to enter the path to the training file */
	QLineEdit* trainingFilePath;

	/*! When pressed this button loads the next stage of the import.
	    Only enabled when the user has entered the file information.*/
	QPushButton* nextButton;


	//=======================  METHODS  =========================
	QString getFilePath(QString fileFilter);
};

#endif//NRMIMPORTDIALOG_H


