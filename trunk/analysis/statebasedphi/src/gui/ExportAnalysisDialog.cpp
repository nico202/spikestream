//SpikeStream includes
#include "ExportAnalysisDialog.h"
#include "Globals.h"
#include "SpikeStreamException.h"
#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextStream>

/*! Constructor */
ExportAnalysisDialog::ExportAnalysisDialog(QWidget* parent) : QDialog(parent){
    //Create layouts
    QVBoxLayout* mainVerticalBox = new QVBoxLayout(this);
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setMargin(10);

    //Add the file selection components
    gridLayout->addWidget(new QLabel("File: "), 0, 0);
    fileLineEdit = new QLineEdit();
    fileLineEdit->setMinimumSize(250, 30);
    gridLayout->addWidget(fileLineEdit, 0, 1);
    QPushButton* fileButt = new QPushButton("Browse");
    connect (fileButt, SIGNAL(clicked()), this, SLOT(getFile()));
    gridLayout->addWidget(fileButt, 0, 2);

    //Add the export type selection combo
    gridLayout->addWidget(new QLabel("Export type: "), 1, 0);
    exportTypeCombo = new QComboBox();
    exportTypeCombo->insertItem("Tab separated fields");
    gridLayout->addWidget(exportTypeCombo, 1, 1);

    //Add the grid layout to the dialog
    mainVerticalBox->addLayout(gridLayout);

    //Add Ok and Cancel buttons
    QHBoxLayout *okCanButtonBox = new QHBoxLayout();
    QPushButton *okPushButton = new QPushButton("Ok", this, "okButton");
    connect(okPushButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    QPushButton *cancelPushButton = new QPushButton("Cancel", this, "cancelButton");
    connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
    okCanButtonBox->addWidget(okPushButton);
    okCanButtonBox->addWidget(cancelPushButton);
    mainVerticalBox->addLayout(okCanButtonBox);
}


/*! Destructor */
ExportAnalysisDialog::~ExportAnalysisDialog(){
}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Checks that inputs are correct and adds them to the analysis info */
void ExportAnalysisDialog::okButtonClicked(){
    //Check file has been entered
    if(fileLineEdit->text().isEmpty()){
	qCritical()<<"No file selected for saving.";
	return;
    }

    if(!Globals::analysisLoaded()){
	qCritical()<<"No analysis loaded; nothing to export.";
	return;
    }


    //FIXME: CHECK IF FILE ALREADY EXISTS


    try{
	qDebug()<<"Exporting analysis to file: "<<fileLineEdit->text()<<". Export type: "<<exportTypeCombo->currentIndex();
	switch(exportTypeCombo->currentIndex()){
	    case 0: exportCommaSeparated(fileLineEdit->text());
	    break;
	    default: throw SpikeStreamException("Export type not recognized.");
	}
    }
    catch(SpikeStreamException& ex){
	qCritical()<<ex.getMessage();
    }

    //If we have reached this point, info should be ok
    accept();
}

/*! When user clicks on 'browse' file dialog is displayed to select the file */
void ExportAnalysisDialog::getFile(){
    QString filePath = getFilePath("*.dat");

    //Fix extension
    if(filePath.indexOf(".") == -1)
	fileLineEdit->setText(filePath + ".dat");
    else
	fileLineEdit->setText(filePath);
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

void ExportAnalysisDialog::exportCommaSeparated(const QString& filePath){
    //Get the data to be exported
    StateBasedPhiAnalysisDao stateDao(Globals::getAnalysisDao()->getDBInfo());
    QList<Complex> complexList = stateDao.getComplexes(Globals::getAnalysisID());

    //Open the file
    QFile outFile(filePath);
    if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text)){
	qCritical()<<"File: "<<filePath<<" could not be opened.";
	return;
    }
    QTextStream outStream(&outFile);

    //Write headers
    outStream<<"Analysis ID\tTime Step\tPhi\tNeuron IDs\n";

    //Write complexes
    foreach(Complex tmpCmplx, complexList){
	outStream<<tmpCmplx.getID()<<"\t";
	outStream<<tmpCmplx.getTimeStep()<<"\t";
	outStream<<tmpCmplx.getPhi()<<"\t";
	outStream<<tmpCmplx.getNeuronIDString()<<"\n";
    }

    //Close the file
    outFile.close();
}



/*! Enables user to enter a file path */
QString ExportAnalysisDialog::getFilePath(QString fileFilter){
    QFileDialog dialog(this);
    dialog.setDirectory(Globals::getWorkingDirectory());
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter( QString("Data files (" + fileFilter + ")") );
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (dialog.exec())
	fileNames = dialog.selectedFiles();
    if(fileNames.size() > 0)
	return fileNames[0];
    else
	return QString("");
}

