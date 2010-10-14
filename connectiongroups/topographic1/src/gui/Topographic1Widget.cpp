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
		return QString("Topographic1 connection group builder");
	}
}


/*! Constructor */
Topographic1Widget::Topographic1Widget(QWidget* parent) : AbstractConnectionWidget(parent) {
	//Construct GUI
	mainVBox = new QVBoxLayout();
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
		checkInput(fromNeuronGrpCombo, "From neuron group has not been set.");
		checkInput(toNeuronGrpCombo, "To neuron group has not been set.");
		checkInput(projectionWidthEdit, "Projection width has not been set.");
		checkInput(projectionLengthEdit, "Projection length has not been set.");
		checkInput(projectionHeightEdit, "Projection height has not been set.");
		checkInput(overlapWidthEdit, "Overlap width has not been set.");
		checkInput(overlapLengthEdit, "Overlap length has not been set.");
		checkInput(overlapHeightEdit, "Overlap height has not been set.");
		checkInput(positionCombo, "Projection position has not been set");
		checkInput(forRevCombo, "Forward or reverse direction has not been set");
		checkInput(connectionPatternCombo, "Connection pattern has not been set");
		checkInput(minWeightEdit, "Minimum weight has not been set.");
		checkInput(maxWeightEdit, "Maximum weight has not been set.");
		checkInput(delayTypeCombo, "Connection pattern has not been set");
		if(delayTypeCombo->currentIndex() == 0)
			checkInput(delayDistanceFactorEdit, "Delay distance factor has not been set.");
		else{
			checkInput(minDelayEdit, "Minimum delay has not been set.");
			checkInput(maxDelayEdit, "Maximum delay has not been set.");
		}
		checkInput(densityEdit, "Density has not been set.");
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
	unsigned int fromNeurGrpID = getNeuronGroupID(fromNeuronGrpCombo->currentText());
	unsigned int toNeurGrpID = getNeuronGroupID(toNeuronGrpCombo->currentText());
	unsigned int synapseType = getSynapseTypeID(synapseTypeCombo->currentText());

	//Store parameters in parameter map
	QHash<QString, double> paramMap;
	paramMap["projection_width"] = Util::getDouble(projectionWidthEdit->text());
	paramMap["projection_length"] = Util::getDouble(projectionLengthEdit->text());
	paramMap["projection_height"] = Util::getDouble(projectionHeightEdit->text());
	paramMap["overlap_width"] = Util::getDouble(overlapWidthEdit->text());
	paramMap["overlap_length"] = Util::getDouble(overlapLengthEdit->text());
	paramMap["overlap_height"] = Util::getDouble(overlapHeightEdit->text());
	paramMap["projection_position"] = positionCombo->currentIndex();
	paramMap["forward_reverse"] = forRevCombo->currentIndex();
	paramMap["connection_pattern"] = connectionPatternCombo->currentIndex();
	paramMap["min_weight"] = Util::getDouble(minWeightEdit->text());
	paramMap["max_weight"] = Util::getDouble(maxWeightEdit->text());
	paramMap["delay_type"] = delayTypeCombo->currentIndex();
	paramMap["delay_distance_factor"] = Util::getDouble(delayDistanceFactorEdit->text());
	paramMap["min_delay"] = Util::getDouble(minDelayEdit->text());
	paramMap["max_delay"] = Util::getDouble(maxDelayEdit->text());
	paramMap["density"] = Util::getDouble(densityEdit->text());

	//Use extracted praameters to construct connection group info
	ConnectionGroupInfo info(0, descriptionEdit->text(), fromNeurGrpID, toNeurGrpID, paramMap, synapseType);
	return info;
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Switches between delay based on distance and range of delays */
void Topographic1Widget::delayTypeChanged(int indx){
	if(indx == 0){
		delayDistanceFactorEdit->setEnabled(true);
		minDelayEdit->setEnabled(false);
		maxDelayEdit->setEnabled(false);
	}
	else if(indx ==1){
		delayDistanceFactorEdit->setEnabled(false);
		minDelayEdit->setEnabled(true);
		maxDelayEdit->setEnabled(true);
	}
	else
		qCritical()<<"Delay type not recognized: "<<indx;
}

/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds a widget to the supplied layout with a label */
void Topographic1Widget::addInputWidget(QWidget* widget, QHBoxLayout* layout, QString label, bool limitWidth, bool lastWidget){
	if(limitWidth)
		widget->setMaximumWidth(80);
	layout->addWidget(new QLabel(label));
	layout->addWidget(widget);
	if(lastWidget){
		layout->addStretch(5);
		mainVBox->addLayout(layout);
		mainVBox->addSpacing(vSpacing);
	}
}


/*! Builds the graphical components */
void Topographic1Widget::buildGUI(QVBoxLayout* mainVBox){
	QGroupBox* mainGroupBox = new QGroupBox("Topographic1 Connection Group Builder", this);

	uint64_t testLong = 0xffffffffffffffff;
	qDebug()<<"Size: "<<sizeof(testLong)<<"; value="<<testLong;

	//Validators for double and integer parameters
	QDoubleValidator* posDoubleValidator = new QDoubleValidator(0.0, 1000000.0, 2, this);
	QDoubleValidator* projectionValidator = new QDoubleValidator(0.1, 1000000.0, 2, this);
	QDoubleValidator* overlapValidator = new QDoubleValidator(0.0, 1000000.0, 2, this);
	QDoubleValidator* weightValidator = new QDoubleValidator(-1.0, 1.0, 3, this);
	QIntValidator* delayValidator = new QIntValidator(0, 10000, this);

	//Add description widget
	QHBoxLayout* descLayout = new QHBoxLayout();
	addInputWidget((QWidget*)(descriptionEdit = new QLineEdit("Undescribed")), descLayout, "Description: ", true, true);

	//Add from and to combos
	QHBoxLayout* fromToLayout = new QHBoxLayout();
	fromNeuronGrpCombo = new QComboBox();
	addNeuronGroups(fromNeuronGrpCombo);
	toNeuronGrpCombo = new QComboBox();
	addNeuronGroups(toNeuronGrpCombo);
	addInputWidget(fromNeuronGrpCombo, fromToLayout, "From: ");
	addInputWidget(toNeuronGrpCombo, fromToLayout, "To: ", false, true);

	//Projection settings
	QHBoxLayout* projLayout1 = new QHBoxLayout();
	projectionWidthEdit = new QLineEdit("10.0");
	projectionWidthEdit->setValidator(projectionValidator);
	addInputWidget(projectionWidthEdit, projLayout1, "Projection width:", true);
	projectionLengthEdit = new QLineEdit("10.0");
	projectionLengthEdit->setValidator(projectionValidator);
	addInputWidget(projectionLengthEdit, projLayout1, "Projection length:", true);
	projectionHeightEdit = new QLineEdit("10.0");
	projectionHeightEdit->setValidator(projectionValidator);
	addInputWidget(projectionHeightEdit, projLayout1, "Projection height:", true, true);

	//Overlap
	QHBoxLayout* overlapLayout = new QHBoxLayout();
	overlapWidthEdit = new QLineEdit("0.0");
	overlapWidthEdit->setValidator(overlapValidator);
	addInputWidget(overlapWidthEdit, overlapLayout, "Overlap width:", true);
	overlapLengthEdit = new QLineEdit("0.0");
	overlapLengthEdit->setValidator(overlapValidator);
	addInputWidget(overlapLengthEdit, overlapLayout, "Overlap length:", true);
	overlapHeightEdit = new QLineEdit("0.0");
	overlapHeightEdit->setValidator(overlapValidator);
	addInputWidget(overlapHeightEdit, overlapLayout, "Overlap height:", true, true);

	//Other stuff
	QHBoxLayout* projLayout2 = new QHBoxLayout();
	positionCombo = new QComboBox();
	fillPositionCombo();
	addInputWidget(positionCombo, projLayout2, "Position:");
	forRevCombo = new QComboBox();
	forRevCombo->addItem("Forward");
	forRevCombo->addItem("Reverse");
	addInputWidget(forRevCombo, projLayout2, "Direction:");
	connectionPatternCombo = new QComboBox();
	fillConnectionPatternCombo();
	addInputWidget(connectionPatternCombo, projLayout2, "Pattern");
	densityEdit = new QLineEdit("1.0");
	densityEdit->setValidator(posDoubleValidator);
	addInputWidget(densityEdit, projLayout2, "Density:", true, true);

	//Weight
	QHBoxLayout* weiLayout = new QHBoxLayout();
	minWeightEdit = new QLineEdit("0.0");
	minWeightEdit->setValidator(weightValidator);
	addInputWidget(minWeightEdit, weiLayout, "Minimum weight:", true);
	maxWeightEdit = new QLineEdit("1.0");
	maxWeightEdit->setValidator(weightValidator);
	addInputWidget(maxWeightEdit, weiLayout, "Maximum weight:", true, true);

	//Delay
	QHBoxLayout* delayLayout = new QHBoxLayout();
	delayTypeCombo = new QComboBox();
	delayTypeCombo->addItem("Distance");
	delayTypeCombo->addItem("Range");
	connect(delayTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(delayTypeChanged(int)));
	addInputWidget(delayTypeCombo, delayLayout, "Delay type:");
	delayDistanceFactorEdit = new QLineEdit("1.0");
	delayDistanceFactorEdit->setValidator(posDoubleValidator);
	addInputWidget(delayDistanceFactorEdit, delayLayout, "Delay distance factor:", true);
	minDelayEdit = new QLineEdit("1");
	minDelayEdit->setEnabled(false);
	minDelayEdit->setValidator(delayValidator);
	addInputWidget(minDelayEdit, delayLayout, "Min delay:", true);
	maxDelayEdit = new QLineEdit("1");
	maxDelayEdit->setEnabled(false);
	maxDelayEdit->setValidator(delayValidator);
	addInputWidget(maxDelayEdit, delayLayout, "Max delay:", true, true);

	//Add connection probability and synapse type
	QHBoxLayout* miscLayout = new QHBoxLayout();
	synapseTypeCombo = new QComboBox();
	addSynapseTypes(synapseTypeCombo);
	addInputWidget(synapseTypeCombo, miscLayout, "Synapse type:", true);

	mainGroupBox->setLayout(mainVBox);
	this->setMinimumSize(800, 400);
}


/*! Fills connection pattern combo with available connection patterns. */
void Topographic1Widget::fillConnectionPatternCombo(){
	connectionPatternCombo->addItem("Gaussian sphere");
	connectionPatternCombo->addItem("Uniform sphere");
	connectionPatternCombo->addItem("Uniform cube");
}


/*! Fills the position combo with top left etc. */
void Topographic1Widget::fillPositionCombo(){
	positionCombo->addItem("Centre");
	positionCombo->addItem("Top left");
	positionCombo->addItem("Top right");
	positionCombo->addItem("Bottom left");
	positionCombo->addItem("Bottom right");
}



