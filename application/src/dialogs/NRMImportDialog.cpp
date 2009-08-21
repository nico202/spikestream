//SpikeStream includes
#include "NRMImportDialog.h"
#include "Globals.h"

//Qt includes
#include <QLayout>
#include <QLabel>
#include <QFileDialog>

/*! Constructor */
NRMImportDialog::NRMImportDialog(QWidget* parent) : QDialog(parent, "NoiseParamDlg", false){

    //Set caption
    this->setCaption("Import NRM Network");

    //Create box to organise dialog
    QVBoxLayout *verticalBox = new QVBoxLayout(this, 2, 2);

    QHBoxLayout *configBox = new QHBoxLayout();
    configBox->addWidget(new QLabel("NRM Configuration File: "));
    configFilePath = new QLineEdit(this);
    configFilePath->setMinimumSize(150, 30);FIXME FIXME
    configBox->addWidget(configFilePath);
    QPushButton* configFileButt = new QPushButton("Browse", this);
    connect (configFileButt, SIGNAL(clicked()), this, SLOT(getConfigFile()));
    configBox->addWidget(configFileButt);
    verticalBox->addLayout(configBox);

    QHBoxLayout *trainingBox = new QHBoxLayout();
    trainingBox->addWidget(new QLabel("NRM Training File: "));
    trainingFilePath = new QLineEdit(this);
    trainingBox->addWidget(trainingFilePath);
    QPushButton* trainingFileButt = new QPushButton("Browse", this);
    connect (trainingFileButt, SIGNAL(clicked()), this, SLOT(getTrainingFile()));
    trainingBox->addWidget(trainingFileButt);
    verticalBox->addLayout(trainingBox);

    QHBoxLayout *buttonBox = new QHBoxLayout();
    nextButton = new QPushButton("Next", this);
    buttonBox->addWidget(nextButton);
    connect (nextButton, SIGNAL(clicked()), this, SLOT(nextButtonPressed()));
    QPushButton* cancelButton = new QPushButton("Cancel", this);
    buttonBox->addWidget(cancelButton);
    connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    verticalBox->addLayout(buttonBox);

}


/*! Destructor */
NRMImportDialog::~NRMImportDialog(){
}


void NRMImportDialog::getConfigFile(){
    QString filePath = getFilePath("*.cfg");
    configFilePath->setText(filePath);
}

void NRMImportDialog::getTrainingFile(){
    QString filePath = getFilePath("*.ntr");
    trainingFilePath->setText(filePath);
}

QString NRMImportDialog::getFilePath(QString fileFilter){
    QFileDialog dialog(this);
    dialog.setDirectory(Globals::getRootDirectory());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter( QString("Configuration files (" + fileFilter + ")") );
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (dialog.exec())
	fileNames = dialog.selectedFiles();
    if(fileNames.size() > 0)
	return fileNames[0];
    else
	return QString("");
}


void NRMImportDialog::nextButtonPressed(){
}







