//SpikeStream includes
#include "NemoParametersDialog.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>


/*! Constructor */
NemoParametersDialog::NemoParametersDialog(const QList<ParameterInfo>& paramInfoList, const QHash<QString, double>& currParamValueMap, QWidget* parent)
	: AbstractParametersEditDialog(paramInfoList, parent) {

	//Create main layout to organize dialog
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	//Add the parameter edit fields and check boxes
	addParameters(mainVBox);

	//Add

	//Sets the values of the parameters
	setParameterValues(currParamValueMap);



	//Add the buttons
	addButtons(mainVBox);


}


/*! Destructor */
NemoParametersDialog::~NemoParametersDialog(){
}
