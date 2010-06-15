//SpikeStream includes
#include "Globals.h"
#include "LoadAnalysisDialog.h"
#include "Util.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QItemSelectionModel>


/*! Constructor */
LoadAnalysisDialog::LoadAnalysisDialog(QWidget* parent, unsigned int analysisType) : QDialog(parent) {
	//Store analysis type to filter results
	this->analysisType = analysisType;

    //Check that both a network and an archive have been loaded
    if(!Globals::networkLoaded() || !Globals::archiveLoaded()){
		qCritical()<<"Analyses are linked to a particular network and archive, which must be loaded first.";
		return;
    }

    QVBoxLayout *mainVerticalBox = new QVBoxLayout(this);

    //Create model and add view
	analysesModel = new AnalysesModel(analysisType);
	QTableView* analysesTableView = new AnalysesTableView(this, analysesModel);
    analysesTableView->setMinimumSize(600, 300);
    mainVerticalBox->addWidget(analysesTableView);

    //Add buttons
    QHBoxLayout* buttonBox = new QHBoxLayout();
    QPushButton* okPushButton = new QPushButton("Ok");
    buttonBox->addWidget(okPushButton);
    QPushButton* deletePushButton = new QPushButton("Delete selected analyses");
    buttonBox->addWidget(deletePushButton);
    QPushButton* cancelPushButton = new QPushButton("Cancel");
    buttonBox->addWidget(cancelPushButton);
    mainVerticalBox->addLayout(buttonBox);

    connect (okPushButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
    connect (deletePushButton, SIGNAL(clicked()), this, SLOT(deleteButtonPressed()));
    connect (cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));

    //Automatically hide when network display changes
    connect(Globals::getEventRouter(), SIGNAL(networkDisplayChangedSignal()), this, SLOT(hide()));
}


/*! Destructor */
LoadAnalysisDialog::~LoadAnalysisDialog(){
}


/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Returns the analysis info. This method should not be invoked if the
    user has cancelled the operation. */
const AnalysisInfo& LoadAnalysisDialog::getAnalysisInfo(){
    if(analysisInfo.getID() == 0)
		throw SpikeStreamException("Analysis info ID is 0. This method should not be called if the user has cancelled selection of analysis.");
    return analysisInfo;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE SLOTS                   -----*/
/*----------------------------------------------------------*/

/*! Stores the currently selected analysis.
    Produces an error message if the user has not selected a single analysis. */
void LoadAnalysisDialog::okButtonPressed(){
    QList<AnalysisInfo> selectedAnalysisList = analysesModel->getSelectedAnalyses();

    //Check that there is exactly one selection
    if(selectedAnalysisList.size() != 1){
		qCritical()<<"A single analysis must be selected for loading.";
		return;
    }

    //Store the selected analysis and accept dialog
    analysisInfo = selectedAnalysisList.at(0);
    this->accept();
}


/*! Deletes the selected analyses after requesting confirmation from user */
void LoadAnalysisDialog::deleteButtonPressed(){
    QList<AnalysisInfo> selectedAnalysisList = analysesModel->getSelectedAnalyses();

    //Check that there is at least one selection
    if(selectedAnalysisList.size() == 0){
		return;
    }

    //Confirm that user wants to delete the analyses
    QMessageBox msgBox;
    msgBox.setText("Deleting Analyses");
    msgBox.setInformativeText("Are you sure that you want to delete " + QString::number(selectedAnalysisList.size()) + " analyses? This step cannot be undone.");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret != QMessageBox::Ok)
		return;

    //Delete the analyses
    foreach(AnalysisInfo anaInfo, selectedAnalysisList){
		Globals::getAnalysisDao()->deleteAnalysis(anaInfo.getID());
    }

    //Reload the model
    analysesModel->reload();
}



