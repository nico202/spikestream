//SpikeStream includes
#include "NemoParametersDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QLayout>


/*! Constructor */
NemoParametersDialog::NemoParametersDialog(const QList<ParameterInfo>& paramInfoList, const QHash<QString, double>& currParamValueMap, const QHash<QString, double>& defaultParamValueMap, QWidget* parent)
	: AbstractParametersEditDialog(paramInfoList, parent) {

	//Store default values of parameters
	this->defaultParameterMap = defaultParamValueMap;

	//Create main layout to organize dialog
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	//Add the parameter edit fields and check boxes
	addParameters(mainVBox);

	//Sets the values of the parameters
	setParameterValues(currParamValueMap);

	//Add the buttons
	addButtons(mainVBox);
}


/*! Destructor */
NemoParametersDialog::~NemoParametersDialog(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

//Override
void NemoParametersDialog::defaultButtonClicked(){
	setParameterValues(defaultParameterMap);
}


//Override
void NemoParametersDialog::okButtonClicked(){
	try{
		//Throws an exception if parameter values are empty
		parameterMap = getParameterValues();
		this->accept();
	}
	catch(SpikeStreamException& ex){
		parameterMap.clear();
		qWarning()<<ex.getMessage();
	}
}

