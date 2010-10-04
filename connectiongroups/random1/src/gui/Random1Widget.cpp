//SpikeStream includes
#include "Random1Widget.h"
#include "Globals.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QGroupBox>
#include <QHash>
#include <QLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QValidator>


//Functions for dynamic library loading
extern "C" {
	/*! Creates a Random1Widget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new Random1Widget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Random1 connection group builder");
	}
}


/*! Constructor */
Random1Widget::Random1Widget(QWidget* parent) : QWidget(parent) {
	//Main vertical box
	QVBoxLayout* mainVBox = new QVBoxLayout();
	QGroupBox* mainGroupBox = new QGroupBox("Random1 Connection Group Builder", this);

	//Validators for double and integer parameters
	QDoubleValidator* doubleValidator = new QDoubleValidator(-1.0, 1000000.0, 2, this);
	QDoubleValidator* weightValidator = new QDoubleValidator(-1.0, 1.0, 3, this);
	QIntValidator* delayValidator = new QIntValidator(0, 10000, this);
	QIntValidator* percentValidator = new QIntValidator(0, 100, this);

	//Add description widget
	descriptionEdit = new QLineEdit("Undescribed");
	QHBoxLayout* descLayout = new QHBoxLayout();
	descLayout->addWidget(new QLabel("Description: "));
	descLayout->addWidget(descriptionEdit);
	mainVBox->addLayout(descLayout);
	mainVBox->addSpacing(10);

	//Add from and to combos
	fromCombo = new QComboBox();
	addNeuronGroups(fromCombo);
	toCombo = new QComboBox();
	addNeuronGroups(toCombo);
	QHBoxLayout* fromToLayout = new QHBoxLayout();
	fromToLayout->addWidget(new QLabel("From: "));
	fromToLayout->addWidget(fromCombo);
	fromToLayout->addWidget(new QLabel(" To: "));
	fromToLayout->addWidget(toCombo);
	fromToLayout->addStretch(10);
	mainVBox->addLayout(fromToLayout);
	mainVBox->addSpacing(10);

	//Weight range 1
	minWeightRange1Edit = new QLineEdit("-1.0");
	minWeightRange1Edit->setValidator(weightValidator);
	maxWeightRange1Edit = new QLineEdit("0.0");
	maxWeightRange1Edit->setValidator(weightValidator);
	weightRange1PercentEdit = new QLineEdit("20");
	weightRange1PercentEdit->setValidator(percentValidator);
	QHBoxLayout* weightRange1Box = new QHBoxLayout();
	weightRange1Box->addWidget(new QLabel("Weight range 1 from: "));
	weightRange1Box->addWidget(minWeightRange1Edit);
	weightRange1Box->addWidget(new QLabel(" to: "));
	weightRange1Box->addWidget(maxWeightRange1Edit);
	weightRange1Box->addWidget(new QLabel(" Proportion weight range 1: "));
	weightRange1Box->addWidget(weightRange1PercentEdit);
	weightRange1Box->addWidget(new QLabel(" %"));
	weightRange1Box->addStretch(10);
	mainVBox->addLayout(weightRange1Box);
	mainVBox->addSpacing(10);

	//Weight range 2
	minWeightRange2Edit = new QLineEdit("0");
	minWeightRange2Edit->setValidator(weightValidator);
	maxWeightRange2Edit = new QLineEdit("1.0");
	maxWeightRange2Edit->setValidator(weightValidator);
	QHBoxLayout* weightRange2Box = new QHBoxLayout();
	weightRange2Box->addWidget(new QLabel("Weight range 2 from: "));
	weightRange2Box->addWidget(minWeightRange2Edit);
	weightRange2Box->addWidget(new QLabel(" to: "));
	weightRange2Box->addWidget(maxWeightRange2Edit);
	weightRange2Box->addStretch(10);
	mainVBox->addLayout(weightRange2Box);
	mainVBox->addSpacing(10);

	//Delay
	minDelayEdit = new QLineEdit("1");
	minDelayEdit->setValidator(delayValidator);
	maxDelayEdit = new QLineEdit("1");
	maxDelayEdit->setValidator(delayValidator);
	QHBoxLayout* delayBox = new QHBoxLayout();
	delayBox->addWidget(new QLabel("Delay (ms) from: "));
	delayBox->addWidget(minDelayEdit);
	delayBox->addWidget(new QLabel(" to: "));
	delayBox->addWidget(maxDelayEdit);
	delayBox->addStretch(10);
	mainVBox->addLayout(delayBox);
	mainVBox->addSpacing(10);

	//Add connection probability and synapse type
	connectionProbabilityEdit = new QLineEdit("1.0");
	connectionProbabilityEdit->setMaximumSize(100, 30);
	connectionProbabilityEdit->setValidator(doubleValidator);
	synapseTypeCombo = new QComboBox();
	addSynapseTypes(synapseTypeCombo);
	QHBoxLayout* miscLayout = new QHBoxLayout();
	miscLayout->addWidget(new QLabel(" Connection probability (0-1): "));
	miscLayout->addWidget(connectionProbabilityEdit);
	miscLayout->addWidget(new QLabel(" Synapse type: "));
	miscLayout->addWidget(synapseTypeCombo);
	miscLayout->addStretch(5);
	mainVBox->addLayout(miscLayout);
	mainVBox->addSpacing(10);

	//Add button
	QHBoxLayout *addButtonBox = new QHBoxLayout();
	QPushButton *addPushButton = new QPushButton("Add");
	addPushButton->setMaximumSize(100, 30);
	connect(addPushButton, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
	addButtonBox->addStretch(10);
	addButtonBox->addWidget(addPushButton);
	mainVBox->addLayout(addButtonBox);

	mainGroupBox->setLayout(mainVBox);
	this->setMinimumSize(800, 300);

	//Create builder thread class
	builderThread = new Random1BuilderThread();
	connect (builderThread, SIGNAL(finished()), this, SLOT(builderThreadFinished()));
	connect(builderThread, SIGNAL( progress(int, int, QString) ), this, SLOT( updateProgress(int, int, QString) ), Qt::QueuedConnection);
}


/*! Destructor */
Random1Widget::~Random1Widget(){
}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Called when add button is clicked.
	Checks the input and adds the specified network to the database */
void Random1Widget::addButtonClicked(){
	//Double check network is loaded
	if(!Globals::networkLoaded()){
		QMessageBox::critical(this, "Random1 Connection Group Builder Error", "No network loaded.", QMessageBox::Ok);
		return;
	}

	//Fix description
	if(descriptionEdit->text().isEmpty())
		descriptionEdit->setText("Undescribed");

	//Check inputs are not empty
	try{
		checkInput(fromCombo, "From neuron group has not been set.");
		checkInput(toCombo, "To neuron group has not been set.");
		checkInput(minWeightRange1Edit, "Min weight range 1 has not been set.");
		checkInput(maxWeightRange1Edit, "Max weight range 1 has not been set.");
		checkInput(weightRange1PercentEdit, "Max weight range 2 has not been set.");
		checkInput(minWeightRange2Edit, "Min weight range 2 has not been set.");
		checkInput(maxWeightRange2Edit, "Max weight range 2 has not been set.");
		checkInput(minDelayEdit, "Min delay has not been set.");
		checkInput(maxDelayEdit, "Max delay has not been set.");
		checkInput(connectionProbabilityEdit, "Density has not been set.");
	}
	catch(SpikeStreamException& ex){
		QMessageBox::warning(this, "Random1 Connection Group Builder", ex.getMessage(), QMessageBox::Ok);
		return;
	}

	//Extract variables
	try {
		unsigned int fromNeurGrpID = getNeuronGroupID(fromCombo->currentText());
		unsigned int toNeurGrpID = getNeuronGroupID(toCombo->currentText());
		unsigned int synapseType = getSynapseTypeID(synapseTypeCombo->currentText());

		//Store parameters in parameter map
		QHash<QString, double> paramMap;
		paramMap["min_weight_range_1"] = Util::getDouble(minWeightRange1Edit->text());
		paramMap["max_weight_range_1"] = Util::getDouble(maxWeightRange1Edit->text());
		paramMap["percent_weight_range_1"] = Util::getDouble(weightRange1PercentEdit->text());
		paramMap["min_weight_range_2"] = Util::getDouble(minWeightRange2Edit->text());
		paramMap["max_weight_range_2"] = Util::getDouble(maxWeightRange2Edit->text());
		paramMap["min_delay"] = Util::getDouble(minDelayEdit->text());
		paramMap["max_delay"] = Util::getDouble(maxDelayEdit->text());
		paramMap["connection_probability"] = Util::getDouble(connectionProbabilityEdit->text());

		//Start thread to add neuron group
		ConnectionGroupInfo info(0, descriptionEdit->text(), fromNeurGrpID, toNeurGrpID, paramMap, synapseType);
		builderThread->startBuildConnectionGroup(info);
		progressDialog = new QProgressDialog("Adding connection group", "Cancel", 0, 100, this, Qt::CustomizeWindowHint);
		progressDialog->setWindowModality(Qt::WindowModal);
		progressDialog->setMinimumDuration(2000);
		progressDialog->setCancelButton(0);//Too complicated to implement cancel sensibly
		updatingProgress = false;
		builderThread->start();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
		return;
	}
}


/*! Called when the builder thread has finished */
void Random1Widget::builderThreadFinished(){
	if(builderThread->isError())
		qCritical()<<builderThread->getErrorMessage();

	//Close progress dialog
	progressDialog->close();

	//Inform other classes that network has changed
	Globals::getEventRouter()->networkChangedSlot();
}


/*! Updates user with feedback about progress with the operation */
void Random1Widget::updateProgress(int stepsCompleted, int totalSteps, QString message){
	//Set flag to avoid multiple calls to progress dialog while it is redrawing
	if(updatingProgress)
		return;
	updatingProgress = true;

	//Check for cancellation
	if(progressDialog->wasCanceled()){
		qCritical()<<"Cuboid plugin does not currently support cancellation of adding connections.";
	}
	//Update progress
	else if(stepsCompleted < totalSteps){
		progressDialog->setValue(stepsCompleted);
		progressDialog->setMaximum(totalSteps);
		progressDialog->setLabelText(message);
	}
	//Progress has finished
	else{
		progressDialog->close();
	}

	//Clear flag to indicate that update of progress is complete
	updatingProgress = false;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds the neuron groups from the current network to the combo box */
void Random1Widget::addNeuronGroups(QComboBox* combo){
	QList<NeuronGroupInfo> neurGrpInfoList = Globals::getNetwork()->getNeuronGroupsInfo();
	foreach(NeuronGroupInfo neurGrpInfo, neurGrpInfoList){
		combo->addItem(neurGrpInfo.getName() + " (" + QString::number(neurGrpInfo.getID()) + ")");
	}
}


/*! Adds synapse types to the specified combo */
void Random1Widget::addSynapseTypes(QComboBox* combo){
	QList<SynapseType> synapseTypesList = Globals::getNetworkDao()->getSynapseTypes();
	foreach(SynapseType synType, synapseTypesList){
		combo->addItem(synType.getDescription() + " (" + QString::number(synType.getID()) + ")");
	}
}


/*! Checks that a combo box has at least one entry */
void Random1Widget::checkInput(QComboBox* combo, const QString& errorMessage){
	if(combo->count() == 0){
		throw SpikeStreamException(errorMessage);
	}
}


/*! Checks that a line edit has a valid input */
void Random1Widget::checkInput(QLineEdit* inputEdit, const QString& errMsg){
	if(inputEdit->text().isEmpty()){
		throw SpikeStreamException(errMsg);
	}
}


/*! Extracts the neuron group ID from the text of a combo box */
unsigned int Random1Widget::getNeuronGroupID(const QString& comboText){
	if(comboText.isEmpty())
		throw SpikeStreamException("Cannot extract a neuron group ID from empty text.");

	QRegExp regExp("[()]");
	return Util::getUInt(comboText.section(regExp, 1, 1));
}


/*! Extracts the synapse type id from the text of a combo box */
unsigned int Random1Widget::getSynapseTypeID(const QString& comboText){
	if(comboText.isEmpty())
		throw SpikeStreamException("Cannot extract a synapse type ID from empty text.");

	QRegExp regExp("[()]");
	return Util::getUInt(comboText.section(regExp, 1, 1));
}


