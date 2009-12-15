//SpikeStream includes
#include "FullResultsModel.h"
#include "Globals.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>

/*! Constructor */
FullResultsModel::FullResultsModel(const AnalysisInfo* analysisInfo, StateBasedPhiAnalysisDao* stateDao) : QAbstractTableModel(){
    //Store references
    this->analysisInfo = analysisInfo;
    this->stateDao = stateDao;
}


/*! Destructor */
FullResultsModel::~FullResultsModel(){
}


/*----------------------------------------------------------*/
/*-----                PUBLIC METHODS                  -----*/
/*----------------------------------------------------------*/

/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int FullResultsModel::columnCount(const QModelIndex&) const{
    return numCols;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
    ready to be displayed in the way requested */
QVariant FullResultsModel::data(const QModelIndex & index, int role) const{
    //Return invalid index if index is invalid or no network loaded
    if (!index.isValid())
	return QVariant();
    if(!Globals::networkLoaded())
	return QVariant();

    //Check rows and columns are in range
    if (index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
	return QVariant();

    //Return appropriate data
    if (role == Qt::DisplayRole){
	//Get pointer to the appropriate Complex class
	const Complex* tmpComplex = &complexList[index.row()];

	if(index.column() == idCol)
	    return tmpComplex->getID();
	if(index.column() == timeStepCol)
	    return tmpComplex->getTimeStep();
	if(index.column() == phiCol)
	    return tmpComplex->getPhi();
	if(index.column() == neurCol)
	    return tmpComplex->getNeuronIDString();
    }

    if (role == Qt::DecorationRole){
	if(index.column() == viewCol ){
	    if( complexDisplayMap.contains( complexList.at(index.row()).getID() ) )
		return QIcon(Globals::getSpikeStreamRoot() + "/images/visible.xpm");
	    return QIcon(Globals::getSpikeStreamRoot() + "/images/hidden.xpm");
	}
    }


    //If we have reached this point ignore request
    return QVariant();
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant FullResultsModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
	return QVariant();

    if (orientation == Qt::Horizontal){
	if(section == idCol)
	    return "ID";
	if(section == timeStepCol)
	    return "Time Step";
	if(section == phiCol)
	    return "Phi";
	if(section == neurCol)
	    return "NeuronIDs";
	if(section == viewCol)
	    return "";
    }

    return QVariant();

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int FullResultsModel::rowCount(const QModelIndex&) const{
    return complexList.size();
}


/*! Sets data in the model */
bool FullResultsModel::setData(const QModelIndex& index, const QVariant&, int) {
    if (!index.isValid() || !Globals::analysisLoaded() )
	return false;
    if (index.row() < 0 || index.row() >= rowCount())
	return false;

    //Change visibility of complex
    if(index.column() == viewCol){
	unsigned int tmpComplexID = complexList[index.row()].getID();
	if(complexDisplayMap.contains(tmpComplexID))
	    setVisibility(complexList[index.row()], false);
	else
	    setVisibility(complexList[index.row()], true);

	//Emit signal that data has changed and return true to indicate data set succesfully.
	emit dataChanged(index, index);
	return true;
    }

    //If we have reached this point no data has been set
    return false;
}


/*! Reloads list of complexes from the database without altering the display settings */
void FullResultsModel::reload(){
    //Clear everything if no analysis is loaded
    if(analysisInfo->getID() == 0){
	clearComplexes();
	reset();
	return;
    }

    //Get the current list of complexes and store ids in a map
    QList<Complex> newComplexList = stateDao->getComplexes(analysisInfo->getID());
    QHash<unsigned int, bool> newComplexMap;
    foreach(Complex tmpCmplx, newComplexList){
	newComplexMap[tmpCmplx.getID()] = true;
    }

    //Hide any complexes that are not in the new list
    foreach(Complex tmpCmplx, complexList){//Work through old complexes
	//Hide complex if it is being shown and not in the new list
	if( complexDisplayMap.contains(tmpCmplx.getID()) && !newComplexMap.contains(tmpCmplx.getID()) ){
	    setVisibility(tmpCmplx, false);
	}
    }

    //Store new list of complexes
    complexList = newComplexList;

    //Signal to views using this model
    reset();
}


/*----------------------------------------------------------*/
/*-------             PRIVATE METHODS                -------*/
/*----------------------------------------------------------*/

/*! Clears all information stored about complexes and resets the highlight map */
void FullResultsModel::clearComplexes(){
    //Hide all complexes
    foreach(Complex tmpCmplx, complexList){
	setVisibility(tmpCmplx, false);
    }

    //Clean up the data structures
    complexList.clear();
    complexDisplayMap.clear();
}


/*! Sets the visibility of the complex with the specified id */
void FullResultsModel::setVisibility(Complex& complex, bool makeVisible){
    //Complex is visible and we want to hide it
    if(complexDisplayMap.contains(complex.getID()) && !makeVisible){
	Globals::getNetworkDisplay()->removeHighlightNeurons(complex.getNeuronIDs());
	complexDisplayMap.remove(complex.getID());
    }
    //Complex is hidden and we want to show it
    else if(!complexDisplayMap.contains(complex.getID()) && makeVisible){
	Globals::getNetworkDisplay()->addHighlightNeurons(complex.getNeuronIDs(), Globals::getNetworkDisplay()->getHighlightNeuronColor());
	complexDisplayMap[complex.getID()] = true;
    }
}


