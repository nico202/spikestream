#include "Globals.h"
#include "LoadAnalysisDialog.h"
#include "Util.h"
#include "XMLParameterParser.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLayout>
#include <QPushButton>
#include <QItemSelectionModel>

/*! Constructor */
LoadAnalysisDialog::LoadAnalysisDialog(QWidget* parent) : QDialog(parent) {
    //Check that both a network and an archive have been loaded
    if(!Globals::networkLoaded() || !Globals::archiveLoaded()){
	qCritical()<<"Analyses are linked to a particular network and archive, which must be loaded first.";
	return;
    }

    QVBoxLayout *mainVerticalBox = new QVBoxLayout(this);

    //Add the table view displaying the current analysis
    analysisTableView = new QTableView();
    unsigned int networkID = Globals::getNetwork()->getID();
    unsigned int archiveID = Globals::getArchive()->getID();
    model = Globals::getAnalysisDao()->getAnalysesTableModel(networkID, archiveID);
    analysisTableView->setModel(model);
    this->setMinimumWidth(400);//FIXME -THIS DOES NOT SEEM TO WORK

    //Select the first row
    selectionModel = analysisTableView->selectionModel();
    QModelIndex firstCellIndex = model->index(0, 0, QModelIndex());
    QItemSelection selection(firstCellIndex, firstCellIndex);
    selectionModel->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);

    //Listen for changes to implement row selection
    connect(selectionModel, SIGNAL(currentChanged (const QModelIndex &, const QModelIndex &)), this, SLOT(selectionChanged( const QModelIndex &, const QModelIndex & )));

    mainVerticalBox->addWidget(analysisTableView);

    QHBoxLayout *okCanButtonBox = new QHBoxLayout();
    QPushButton *okPushButton = new QPushButton("Ok", this, "okButton");
    QPushButton *cancelPushButton = new QPushButton("Cancel", this, "cancelButton");
    okCanButtonBox->addWidget(okPushButton);
    okCanButtonBox->addWidget(cancelPushButton);
    mainVerticalBox->addLayout(okCanButtonBox);

    connect (okPushButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
    connect (cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
}


/*! Destructor */
LoadAnalysisDialog::~LoadAnalysisDialog(){
}


void LoadAnalysisDialog::okButtonPressed(){
    QModelIndexList indexes = analysisTableView->selectionModel()->selectedIndexes();

    //Check that there is at least one selection
    if(indexes.size() == 0){
	qCritical()<<"Selection is missing. Something has gone wrong here!";
	reject();
    }

    //Extract the information about the analysis from the model
    QModelIndex tmpIndex = model->index(indexes[0].row(), 0, QModelIndex());
    unsigned int analysisID = Util::getUInt(model->data(tmpIndex).toString());
    tmpIndex = model->index(indexes[0].row(), 1, QModelIndex());
    QDateTime dateTime = QDateTime::fromString(model->data(tmpIndex).toString());
    tmpIndex = model->index(indexes[0].row(), 2, QModelIndex());
    QString description = model->data(tmpIndex).toString();
    tmpIndex = model->index(indexes[0].row(), 3, QModelIndex());
    QString paramString = model->data(tmpIndex).toString();
    QHash<QString, double> paramMap;
    if(paramString != ""){
	XMLParameterParser parser;
	paramMap = parser.getParameterMap(paramString);
    }
    tmpIndex = model->index(indexes[0].row(), 4, QModelIndex());
    unsigned int analysisType = Util::getUInt(model->data(tmpIndex).toString());

    //Create class holding all of this information
    analysisInfo = AnalysisInfo(
	analysisID,
	Globals::getNetwork()->getID(),
	Globals::getArchive()->getID(),
	dateTime,
	description,
	paramMap,
	analysisType
    );

    this->accept();
}


void LoadAnalysisDialog::selectionChanged( const QModelIndex & currentIndex, const QModelIndex&){
    QModelIndex fromIndex = model->index(currentIndex.row(), 0, QModelIndex());
    QModelIndex toIndex = model->index(currentIndex.row(), 4, QModelIndex());

    QItemSelection selection(fromIndex, toIndex);
    selectionModel->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows | QItemSelectionModel::Current );
    qDebug()<<"SELECTION SIZE: "<<selectionModel->selectedIndexes().size();
}


const AnalysisInfo& LoadAnalysisDialog::getAnalysisInfo(){
    return analysisInfo;
}


