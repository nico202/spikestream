//SpikeStream includes
#include "Globals.h"
#include "SynapseParametersEditDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;


/*! Constructor */
SynapseParametersEditDialog::SynapseParametersEditDialog(const ConnectionGroupInfo& conGrpInfo, const QList<ParameterInfo>& paramInfoList, const QHash<QString, double>& currParamValueMap, QWidget* parent)
		: AbstractParametersEditDialog(paramInfoList, parent) {

	//Store the connection group info for when we update the parameters
	this->conGrpInfo = conGrpInfo;

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
SynapseParametersEditDialog::~SynapseParametersEditDialog(){
}


/*--------------------------------------------------------*/
/*-------              PROTECTED SLOTS             -------*/
/*--------------------------------------------------------*/

/*! Loads up the default values for the parameters */
void SynapseParametersEditDialog::defaultButtonClicked(){
	try{
		setParameterValues(Globals::getNetworkDao()->getDefaultSynapseParameters(conGrpInfo.getSynapseTypeID()));
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		this->accept();
	}
}


/*! Stores the parameter values in the database */
void SynapseParametersEditDialog::okButtonClicked(){
	try{
		QHash<QString, double> paramMap = getParameterValues();

		//Set parameters in the database
		Globals::getNetworkDao()->setSynapseParameters(conGrpInfo, paramMap);

		//Set parameters in the network object stored in memory
		Globals::getNetwork()->getConnectionGroup(conGrpInfo.getID())->setParameters(paramMap);

		//Close dialog
		this->accept();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}
