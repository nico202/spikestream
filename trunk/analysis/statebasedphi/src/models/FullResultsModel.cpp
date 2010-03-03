//SpikeStream includes
#include "FullResultsModel.h"
#include "Globals.h"
#include "GlobalVariables.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QIcon>

/*! Constructor */
FullResultsModel::FullResultsModel(const AnalysisInfo* analysisInfo, StateBasedPhiAnalysisDao* stateDao) : QAbstractTableModel(){
    //Store references
    this->analysisInfo = analysisInfo;
    this->stateDao = stateDao;

    //initialise variables
    complexDisplayIndex = -1;
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
			if( complexDisplayIndex == index.row() )
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


/*! Hides the results of the analysis */
void FullResultsModel::hideAnalysisResults(){
//	//Hide visible complex
//	setVisibleComplex(-1);
//
//	//Inform view associated with this model.
//	reset();
}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int FullResultsModel::rowCount(const QModelIndex&) const{
    return complexList.size();
}


/*! Sets data in the model */
bool FullResultsModel::setData(const QModelIndex& index, const QVariant&, int) {
	if (!index.isValid() || !Globals::isAnalysisLoaded(STATE_BASED_PHI_ANALYSIS) )
		return false;
    if (index.row() < 0 || index.row() >= rowCount())
		return false;

    //Change visibility of complex
    if(index.column() == viewCol){
		setVisibleComplex(index.row());

        //Call reset to reload table becaus other rows could have been affected
        reset();
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

    //Get the current list of complexes
    QList<Complex> newComplexList = stateDao->getComplexes(analysisInfo->getID());

    //Get Id of currently displayed complex
    if(complexDisplayIndex > 0){
		unsigned int visComplexID = complexList[complexDisplayIndex].getID();

		//Determine if this complex is in the new list
		int newComplexDisplayIndex = -1;
		for(int i=0; i<newComplexList.size(); ++i){
			if(visComplexID == newComplexList[i].getID()){
				newComplexDisplayIndex = i;
				break;
			}
		}

		//If complex could not be found, hide complex
		if(newComplexDisplayIndex < 0){
			setVisibility(complexList[complexDisplayIndex], false);
			complexDisplayIndex = -1;
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
    //Hide visible complex
	if(complexDisplayIndex >= 0)
		setVisibility(complexList[complexDisplayIndex], false);

    //Clean up the data structures
    complexList.clear();
    complexDisplayIndex = -1;
}


void FullResultsModel::setVisibleComplex(int index){
    if(index >= complexList.size())
		throw SpikeStreamException("Complex display index is out of range.");

	//No complexes visible and index does not indicate one to display
	if(index < 0 && complexDisplayIndex < 0)
		return;


    //Hide complex if it is already visible
	if(index == complexDisplayIndex){
		setVisibility(complexList[complexDisplayIndex], false);
        complexDisplayIndex = -1;
    }
    //Hide any currently visible complex
    else if(index < 0 && complexDisplayIndex > 0){
		setVisibility(complexList[complexDisplayIndex], false);
		complexDisplayIndex = -1;
    }
    //Different column clicked
    else{
        //Hide current column
        if(complexDisplayIndex >=0)
            setVisibility(complexList[complexDisplayIndex], false);
		setVisibility(complexList[index], true);
		complexDisplayIndex = index;
    }
}


/*! Sets the visibility of the complex with the specified id */
void FullResultsModel::setVisibility(Complex& complex, bool makeVisible){
    //Hide complex
    if(!makeVisible){
		Globals::getNetworkDisplay()->removeHighlightNeurons(complex.getNeuronIDs());
    }
    //Show complex
    else {
		Globals::getNetworkDisplay()->addHighlightNeurons(complex.getNeuronIDs(), Globals::getNetworkDisplay()->getHighlightNeuronColor());
    }
}


