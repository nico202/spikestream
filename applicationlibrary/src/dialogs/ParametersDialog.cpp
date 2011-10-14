//SpikeStream includes
#include "Globals.h"
#include "ParametersDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QScrollArea>


/*! Constructor */
ParametersDialog::ParametersDialog(const QList<ParameterInfo>& paramInfoList, const QHash<QString, double>& defaultParameterMap, const QHash<QString, double>& currentParameterMap, QWidget* parent)
		: AbstractParametersEditDialog(paramInfoList, parent) {

	//Store map with default values
	this->defaultParameterMap = defaultParameterMap;

	//Scroll area for parameters
	QScrollArea* scrollArea = new QScrollArea(this);
	QWidget* parameterHoldingWidget = new QWidget();

	//Create layout to organize widget within scroll area
	QVBoxLayout* mainVBox = new QVBoxLayout(parameterHoldingWidget);

	//Add the parameter edit fields
	addParameters(mainVBox);

	//Sets the values of the parameters
	setParameterValues(currentParameterMap);

	//Add the buttons
	addButtons(mainVBox);

	scrollArea->setWidget(parameterHoldingWidget);
	scrollArea->setMinimumSize(800, 500);
	this->setMinimumSize(800,500);
	this->setMaximumSize(800,500);
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
