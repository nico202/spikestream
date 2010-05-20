//SpikeStream includes
#include "Globals.h"
#include "NeuronParametersEditDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Constructor */
NeuronParametersEditDialog::NeuronParametersEditDialog(const NeuronGroupInfo& neurGrpInfo, const QList<ParameterInfo>& paramInfoList, const QHash<QString, double>& currParamValueMap, QWidget* parent)
		: AbstractParametersEditDialog(paramInfoList, parent) {

	//Store the neuron group info for when we update the parameters
	this->neurGrpInfo = neurGrpInfo;

	//Create layout to organize dialog
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	//Add the parameter edit fields
	addParameters(mainVBox);

	//Sets the values of the parameters
	setParameterValues(currParamValueMap);

	//Add the buttons
	addButtons(mainVBox);
}


/*! Destructor */
NeuronParametersEditDialog::~NeuronParametersEditDialog(){
}


/*--------------------------------------------------------*/
/*-------              PROTECTED SLOTS             -------*/
/*--------------------------------------------------------*/

/*! Loads up the default values for the parameters */
void NeuronParametersEditDialog::defaultButtonClicked(){
	try{
		setParameterValues(Globals::getNetworkDao()->getDefaultNeuronParameters(neurGrpInfo.getNeuronTypeID()));
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		this->accept();
	}
}


/*! Stores the parameter values in the database */
void NeuronParametersEditDialog::okButtonClicked(){
	try{
		QHash<QString, double> paramMap = getParameterValues();

		//Set parameters in the database
		Globals::getNetworkDao()->setNeuronParameters(neurGrpInfo, paramMap);

		//Set parameters in the network object stored in memory
		Globals::getNetwork()->getNeuronGroup(neurGrpInfo.getID())->setParameters(paramMap);

		//Close dialog
		this->accept();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}
