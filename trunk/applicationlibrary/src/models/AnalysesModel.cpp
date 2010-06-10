//SpikeStream includes
#include "Globals.h"
#include "NetworkDisplay.h"
#include "AnalysesModel.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>


/*! Constructor */
AnalysesModel::AnalysesModel(unsigned int analysisType) : QAbstractTableModel(){
	this->analysisType = analysisType;
	reload();
}


/*! Destructor */
AnalysesModel::~AnalysesModel(){
}


/*------------------------------------------------------------*/
/*------               PUBLIC METHODS                   ------*/
/*------------------------------------------------------------*/

/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int AnalysesModel::columnCount(const QModelIndex&) const{
    return numCols;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
    ready to be displayed in the way requested */
QVariant AnalysesModel::data(const QModelIndex & index, int role) const{
    //Return invalid index if index is invalid
    if (!index.isValid())
	return QVariant();

    //Check rows and columns are in range
    if (index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
	return QVariant();

    //Return appropriate data
    if (role == Qt::DisplayRole){
	if(index.column() == idCol)
	    return analysisInfoList[index.row()].getID();
	if(index.column() == netIDCol)
	    return analysisInfoList[index.row()].getNetworkID();
	if(index.column() == archIDCol)
	    return analysisInfoList[index.row()].getArchiveID();
	if(index.column() == timeCol)
	    return analysisInfoList[index.row()].getStartDateTime();
	if(index.column() == descCol)
	    return analysisInfoList[index.row()].getDescription();
	if(index.column() == typeCol)
	    return analysisInfoList[index.row()].getAnalyisType();
    }

    if (role == Qt::DecorationRole){
	if(index.column() == paramCol){
	    return QIcon(Globals::getSpikeStreamRoot() + "/images/view_parameters.xpm");
	}
    }

    if(role == Qt::CheckStateRole){
	if(index.column() == selectCol ){
	    if(selectionMap.contains(index.row()))
		return Qt::Checked;
	    return Qt::Unchecked;
	}
    }

    //If we have reached this point ignore request
    return QVariant();
}


/*! Returns a list of the currently selected analyses */
QList<AnalysisInfo> AnalysesModel::getSelectedAnalyses(){
    QList<AnalysisInfo> tmpList;
    for(int i=0; i<analysisInfoList.size(); ++i){
	if(selectionMap.contains(i))
	    tmpList.append(analysisInfoList.at(i));
    }
    return tmpList;
}


/*! Sets data in the model. */
bool AnalysesModel::setData(const QModelIndex& index, const QVariant&, int) {
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
	return false;

    //Change selection of archive
    if(index.column() == selectCol){
	//Toggle selection of this row
	if(selectionMap.contains(index.row()))
	    selectionMap.remove(index.row());
	else
	    selectionMap[index.row()] = true;

	//Emit signal that data has changed and return true to indicate data set succesfully.
	emit dataChanged(index, index);
	return true;
    }

    //If we have reached this point no data has been set
    return false;
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant AnalysesModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
	return QVariant();

    if (orientation == Qt::Horizontal){
	if(section == selectCol)
	    return "";
	if(section == idCol)
	    return "ID";
	if(section == netIDCol)
	    return "Network";
	if(section == archIDCol)
	    return "Archive";
	if(section == timeCol)
	    return "Time";
	if(section == descCol)
	    return "Description";
	if(section == paramCol)
	    return "Parameters";
	if(section == typeCol)
	    return "Type";
    }

    return QVariant();

}


/*! Reloads information about the analyses currently in the database */
void AnalysesModel::reload(){
    if(!Globals::networkLoaded() || !Globals::archiveLoaded())
	throw SpikeStreamException("This dialog should not be invokable when network or archive is not loaded.");

	analysisInfoList = Globals::getAnalysisDao()->getAnalysesInfo(Globals::getNetwork()->getID(), Globals::getArchive()->getID(), analysisType);
    selectionMap.clear();
}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int AnalysesModel::rowCount(const QModelIndex&) const{
    return analysisInfoList.size();
}



