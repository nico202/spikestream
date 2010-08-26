//SpikeStream includes
#include "Globals.h"
#include "ParametersDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Constructor */
ParametersDialog::ParametersDialog(const QList<ParameterInfo>& paramInfoList, const QHash<QString, double>& defaultParameterMap, const QHash<QString, double>& currentParameterMap, QWidget* parent)
		: AbstractParametersEditDialog(paramInfoList, parent) {

	//Store map with default values
	this->defaultParameterMap = defaultParameterMap;

	//Create layout to organize dialog
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	//Add the parameter edit fields
	addParameters(mainVBox);

	//Sets the values of the parameters
	setParameterValues(currentParameterMap);

	//Add the buttons
	addButtons(mainVBox);
}


/*! Destructor */
ParametersDialog::~ParametersDialog(){
}


/*--------------------------------------------------------*/
/*-------              PROTECTED SLOTS             -------*/
/*--------------------------------------------------------*/

/*! Loads up the default values for the parameters */
void ParametersDialog::defaultButtonClicked(){
	try{
		setParameterValues(defaultParameterMap);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		this->accept();
	}
}


/*! Stores the parameter values in the database */
void ParametersDialog::okButtonClicked(){
	try{
		currentParameterMap = getParameterValues();

		//Close dialog
		this->accept();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}
