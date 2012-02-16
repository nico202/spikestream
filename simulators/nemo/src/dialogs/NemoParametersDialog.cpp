//SpikeStream includes
#include "NemoParametersDialog.h"
#include "ParametersDialog.h"
#include "SpikeStreamException.h"
#include "STDPFunctions.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QPushButton>

//Other includes
#include "nemo.h"


#define NO_CUDA_DEVICES_TEXT "No CUDA devices available."

/*! Constructor */
NemoParametersDialog::NemoParametersDialog(nemo_configuration_t nemoConfig, unsigned stdpFunctionID, QWidget* parent) : QDialog(parent) {

	//Store current nemo config and create a default nemo config
	this->currentNemoConfig = nemoConfig;
	defaultNemoConfig = nemo_new_configuration();
	this->stdpFunctionID = stdpFunctionID;

	//Create layouts to organize dialog
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout();

	//Tracks rows added.
	int rowCntr = 0;

	//Nemo version
	QString versionStr = "Nemo version ";
	gridLayout->addWidget(new QLabel(versionStr + nemo_version()), rowCntr, 0);
	++rowCntr;

	//Combo box to select backend
	backendCombo = new QComboBox();
	backendCombo->addItem("CUDA Hardware");
	backendCombo->addItem("CPU");
	gridLayout->addWidget(new QLabel("Backend: "), rowCntr, 0);
	gridLayout->addWidget(backendCombo, rowCntr, 1);
	connect(backendCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(backendChanged(int)));
	++rowCntr;

	//CUDA device list
	cudaDeviceCombo = new QComboBox();
	getCudaDevices(cudaDeviceCombo);
	cudaDeviceLabel = new QLabel("CUDA device: ");
	gridLayout->addWidget(cudaDeviceLabel, rowCntr, 0);
	gridLayout->addWidget(cudaDeviceCombo, rowCntr, 1);
	++rowCntr;

	//Add combo with STDP functions and button to edit parameters
	stdpCombo = new QComboBox();
	getStdpFunctions(stdpCombo);
	gridLayout->addWidget(stdpCombo, rowCntr, 0);
	QPushButton* stdpParamButton = new QPushButton("Parameters");
	connect(stdpParamButton, SIGNAL(clicked()), this, SLOT(stdpParameterButtonClicked()));
	gridLayout->addWidget(stdpParamButton, rowCntr, 1);
	++rowCntr;

	//Sets the fields to match the config
	loadParameters(currentNemoConfig);
	if((int)stdpFunctionID > stdpCombo->count())
		throw SpikeStreamException("STDP function ID is out of range: " + QString::number(stdpFunctionID));
	stdpCombo->setCurrentIndex(stdpFunctionID);

	//Add the grid layout
	mainVBox->addLayout(gridLayout);

	//Add the buttons
	addButtons(mainVBox);
}


/*! Destructor */
NemoParametersDialog::~NemoParametersDialog(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Called when the backend combo changes. Shows or hides appropriate inputs
	for CUDA or CPU backends. */
void NemoParametersDialog::backendChanged(int index){
	if(index == 0){
		cudaDeviceLabel->show();
		cudaDeviceCombo->show();
	}
	else if(index == 1){
		cudaDeviceLabel->hide();
		cudaDeviceCombo->hide();
	}
}


/*! Resets the inputs with the default parameters.  */
void NemoParametersDialog::defaultButtonClicked(){
	loadParameters(defaultNemoConfig);

	//Set STDP function
	stdpCombo->setCurrentIndex(0);//0 is the default
}


/*! Tests the parameters and stores them in the parameter map.
	Closes the dialog if everything is ok.  */
void NemoParametersDialog::okButtonClicked(){
	try{
		//Throws an exception if parameter values are empty or invalid
		storeParameterValues();
		this->accept();
	}
	catch(SpikeStreamException& ex){
		qWarning()<<ex.getMessage();
	}
}


/*! Sets the parameters for the STDP function */


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds cancel, load defaults, and ok button to the supplied layout. */
void NemoParametersDialog::addButtons(QVBoxLayout* mainVLayout){
	QHBoxLayout *buttonBox = new QHBoxLayout();
	QPushButton* cancelButton = new QPushButton("Cancel");
	buttonBox->addWidget(cancelButton);
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QPushButton* defaultsButton = new QPushButton("Load defaults");
	buttonBox->addWidget(defaultsButton);
	connect (defaultsButton, SIGNAL(clicked()), this, SLOT(defaultButtonClicked()));
	QPushButton* okButton = new QPushButton("Ok");
	buttonBox->addWidget(okButton);
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
	mainVLayout->addLayout(buttonBox);
}


/*! Checks the output from a nemo function call and throws exception if there is an error */
void NemoParametersDialog::checkNemoOutput(nemo_status_t result, const QString& errorMessage){
	if(result != NEMO_OK)
		throw SpikeStreamException(errorMessage + ": " + nemo_strerror());
}


/*! Loads up a list of the currently available CUDA devices */
void NemoParametersDialog::getCudaDevices(QComboBox* combo){
	unsigned numCudaDevices = 0;
	nemo_status_t result = nemo_cuda_device_count(&numCudaDevices);
	if(result != NEMO_OK){
		//combo->addItem("CUDA error: " + QString(nemo_strerror()));
		combo->addItem(NO_CUDA_DEVICES_TEXT);
		return;
	}

	if(numCudaDevices == 0){
		combo->addItem(NO_CUDA_DEVICES_TEXT);
		return;
	}

	for(unsigned i=0; i<numCudaDevices; ++i){
		const char* devDesc;
		checkNemoOutput(nemo_cuda_device_description(i, &devDesc), "Error getting device description.");
		combo->addItem(devDesc);
	}
}


/*! Loads up all of the STDP functions */
void NemoParametersDialog::getStdpFunctions(QComboBox *combo){
	QList<unsigned> functionIDs = STDPFunctions::getFunctionIDs();
	for(int i=0; i<functionIDs.size(); ++i){
		combo->addItem(STDPFunctions::getFunctionDescription(functionIDs.at(i)));
	}
	combo->setCurrentIndex(stdpFunctionID);
}


/*! Loads parameters from the supplied nemo configuration into the graphical components of the dialog. */
void NemoParametersDialog::loadParameters(nemo_configuration_t config){
	//Backend
	backend_t backend;
	int index = 0;
	checkNemoOutput(nemo_backend(config, &backend), "Failed to get NeMo backend.");
	if(backend == NEMO_BACKEND_CUDA)
		index = 0;
	else if (backend == NEMO_BACKEND_CPU)
		index = 1;
	else
		throw SpikeStreamException("Backend not recognized: " + QString::number(backend));

	/* Set backend combo to index and call function to make sure inputs are correctly hidden or shown
	   It may not change the current index, so need to call this function */
	backendCombo->setCurrentIndex(index);
	backendChanged(index);

	//CUDA device
	int cudaDev;
	checkNemoOutput(nemo_cuda_device(config, &cudaDev), "Error getting CUDA device from NeMo");
	if(cudaDev > cudaDeviceCombo->count()){
		throw SpikeStreamException("CUDA device out of range: " + QString::number(cudaDev));
	}
	if(cudaDev < 0)
		cudaDeviceCombo->setCurrentIndex(0);
	else
		cudaDeviceCombo->setCurrentIndex(cudaDev);
}


/*! Saves the parameter values to the parameter map.
	The validity of the values is tested using NeMo and an exception is thrown if they cannot be used. */
void NemoParametersDialog::storeParameterValues(){
	//Set hardware configuration
	if(backendCombo->currentIndex() == 0){//CUDA
		if(cudaDeviceCombo->currentText() == NO_CUDA_DEVICES_TEXT)
			throw SpikeStreamException("Cannot use CUDA - no CUDA devices are available.");
		checkNemoOutput(nemo_set_cuda_backend(currentNemoConfig, cudaDeviceCombo->currentIndex()), "Failed to set CUDA device: ");
	}
	else if(backendCombo->currentIndex() == 1){//CPU
		checkNemoOutput(nemo_set_cpu_backend(currentNemoConfig), "Failed to set backend to CPU: ");
	}
	else{
		throw SpikeStreamException("Backend combo index not recognized.");
	}

	//If we have reached this point, hardware configuration is ok

	//STDP function
	stdpFunctionID = stdpCombo->currentIndex();
}


/*! Shows a dialog that enables the user to edit the parameters of an STDP function. */
void NemoParametersDialog::stdpParameterButtonClicked(){
	try{
		stdpFunctionID = stdpCombo->currentIndex();

		ParametersDialog paramDialog(
				STDPFunctions::getParameterInfoList(stdpFunctionID),
				STDPFunctions::getDefaultParameters(stdpFunctionID),
				STDPFunctions::getParameters(stdpFunctionID),
				this
		);
		if(paramDialog.exec() == QDialog::Accepted){
			QHash<QString, double> newParameters = paramDialog.getParameters();
			STDPFunctions::setParameters(stdpFunctionID, newParameters);
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}



