//SpikeStream includes
#include "Topographic1Widget.h"
#include "Globals.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "Topographic1BuilderThread.h"
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
		return new Topographic1Widget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Random1 connection group builder");
	}
}


/*! Constructor */
Topographic1Widget::Topographic1Widget(QWidget* parent) : AbstractConnectionWidget(parent) {
	//Construct GUI
	QVBoxLayout* mainVBox = new QVBoxLayout();
	buildGUI(mainVBox);

	//Add button
	QHBoxLayout *addButtonBox = new QHBoxLayout();
	QPushButton *addPushButton = new QPushButton("Add");
	addPushButton->setMaximumSize(100, 30);
	connect(addPushButton, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
	addButtonBox->addStretch(10);
	addButtonBox->addWidget(addPushButton);
	mainVBox->addLayout(addButtonBox);

	//Create connection builder
	connectionBuilder = new Topographic1BuilderThread();
	connect (connectionBuilder, SIGNAL(finished()), this, SLOT(builderThreadFinished()));
	connect(connectionBuilder, SIGNAL( progress(int, int, QString) ), this, SLOT( updateProgress(int, int, QString) ), Qt::QueuedConnection);
}


/*! Destructor */
Topographic1Widget::~Topographic1Widget(){
}


/*----------------------------------------------------------*/
/*-----               PROTECTED METHODS                -----*/
/*----------------------------------------------------------*/

//Overridden
bool Topographic1Widget::checkInputs(){
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
		return false;
	}

	//Inputs are ok
	return true;
}


//Override
ConnectionGroupInfo Topographic1Widget::getConnectionGroupInfo(){
	//Extract variables
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

	//Use extracted praameters to construct connection group info
	ConnectionGroupInfo info(0, descriptionEdit->text(), fromNeurGrpID, toNeurGrpID, paramMap, synapseType);
	return info;
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Builds the graphical components */
void Topographic1Widget::buildGUI(QVBoxLayout* mainVBox){
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

	mainGroupBox->setLayout(mainVBox);
	this->setMinimumSize(800, 300);
}

