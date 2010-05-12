//SpikeStream includes
#include "CuboidWidget.h"
#include "Globals.h"
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"
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
	/*! Creates a AleksanderNetworksWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new CuboidWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("Cuboid neuron group builder");
	}
}


/*! Constructor */
CuboidWidget::CuboidWidget(QWidget* parent) : QWidget(parent) {
	//Main vertical box
	QVBoxLayout* mainVBox = new QVBoxLayout();
	QGroupBox* mainGroupBox = new QGroupBox("Cuboid Neuron Group Builder", this);

	//Validators for double and integer parameters
	QDoubleValidator* doubleValidator = new QDoubleValidator(0.0, 1.0, 2, this);
	QIntValidator* posValidator = new QIntValidator(-1000000, 1000000, this);
	QIntValidator* positiveIntValidator = new QIntValidator(0, 1000000, this);

	//Add name and description widgets
	nameEdit = new QLineEdit("Unnamed");
	descriptionEdit = new QLineEdit("Undescribed");
	QHBoxLayout* nameDescLayout = new QHBoxLayout();
	nameDescLayout->addWidget(new QLabel("Name: "));
	nameDescLayout->addWidget(nameEdit);
	nameDescLayout->addWidget(new QLabel("Description: "));
	nameDescLayout->addWidget(descriptionEdit);
	mainVBox->addLayout(nameDescLayout);
	mainVBox->addSpacing(10);

	//Add position input widgets
	xPosEdit = new QLineEdit("1");
	xPosEdit->setMaximumSize(100 , 30);
	xPosEdit->setValidator(posValidator);
	yPosEdit = new QLineEdit("1");
	yPosEdit->setMaximumSize(100 , 30);
	yPosEdit->setValidator(posValidator);
	zPosEdit = new QLineEdit("1");
	zPosEdit->setMaximumSize(100 , 30);
	zPosEdit->setValidator(posValidator);
	QHBoxLayout* positionLayout = new QHBoxLayout();
	positionLayout->addWidget(new QLabel("Position. x: "));
	positionLayout->addWidget(xPosEdit);
	positionLayout->addWidget(new QLabel(" y: "));
	positionLayout->addWidget(yPosEdit);
	positionLayout->addWidget(new QLabel(" z: "));
	positionLayout->addWidget(zPosEdit);
	positionLayout->addStretch(5);
	mainVBox->addLayout(positionLayout);
	mainVBox->addSpacing(10);

	//Add width, length and height
	widthEdit = new QLineEdit("10");
	widthEdit->setMaximumSize(100, 30);
	widthEdit->setValidator(positiveIntValidator);
	lengthEdit = new QLineEdit("10");
	lengthEdit->setMaximumSize(100, 30);
	lengthEdit->setValidator(positiveIntValidator);
	heightEdit = new QLineEdit("10");
	heightEdit->setMaximumSize(100, 30);
	heightEdit->setValidator(positiveIntValidator);
	QHBoxLayout* sizeLayout = new QHBoxLayout();
	sizeLayout->addWidget(new QLabel("Width (X axis): "));
	sizeLayout->addWidget(widthEdit);
	sizeLayout->addWidget(new QLabel(" Length (Y axis): "));
	sizeLayout->addWidget(lengthEdit);
	sizeLayout->addWidget(new QLabel(" Height (Z axis): "));
	sizeLayout->addWidget(heightEdit);
	sizeLayout->addStretch(5);
	mainVBox->addLayout(sizeLayout);
	mainVBox->addSpacing(10);

	//Add spacing, density and neuron type inputs
	spacingEdit = new QLineEdit("1");
	spacingEdit->setMaximumSize(100, 30);
	spacingEdit->setValidator(positiveIntValidator);
	densityEdit = new QLineEdit("1.0");
	densityEdit->setMaximumSize(100, 30);
	densityEdit->setValidator(doubleValidator);
	neuronTypeCombo = new QComboBox();
	addNeuronTypes(neuronTypeCombo);
	QHBoxLayout* miscLayout = new QHBoxLayout();
	miscLayout->addWidget(new QLabel("Spacing (neurons): "));
	miscLayout->addWidget(spacingEdit);
	miscLayout->addWidget(new QLabel(" Density (0-1): "));
	miscLayout->addWidget(densityEdit);
	miscLayout->addWidget(new QLabel(" Neuron type: "));
	miscLayout->addWidget(neuronTypeCombo);
	miscLayout->addStretch(5);
	mainVBox->addLayout(miscLayout);
	mainVBox->addSpacing(5);

	//Add button
	QHBoxLayout *addButtonBox = new QHBoxLayout();
	QPushButton *addPushButton = new QPushButton("Add");
	addPushButton->setMaximumSize(100, 30);
	connect(addPushButton, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
	addButtonBox->addStretch(10);
	addButtonBox->addWidget(addPushButton);
	mainVBox->addLayout(addButtonBox);

	mainGroupBox->setLayout(mainVBox);
	this->setMinimumSize(650, 210);

	//Create builder thread class
	builderThread = new CuboidBuilderThread();
	connect (builderThread, SIGNAL(finished()), this, SLOT(builderThreadFinished()));
	connect(builderThread, SIGNAL( progress(int, int) ), this, SLOT( updateProgress(int, int) ) );
}


/*! Destructor */
CuboidWidget::~CuboidWidget(){
}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Called when add button is clicked.
	Checks the input and adds the specified network to the database */
void CuboidWidget::addButtonClicked(){
	//Double check network is loaded
	if(!Globals::networkLoaded()){
		QMessageBox::critical(this, "Cuboid Neuron Group Builder Error", "No network loaded.", QMessageBox::Ok);
		return;
	}

	//Fix name and description
	if(nameEdit->text().isEmpty())
		nameEdit->setText("Unnamed");
	if(descriptionEdit->text().isEmpty())
		descriptionEdit->setText("Undescribed");

	//Check inputs are not empty
	try{
		checkInput(xPosEdit, "X position has not been set.");
		checkInput(yPosEdit, "Y position has not been set.");
		checkInput(zPosEdit, "Z position has not been set.");
		checkInput(widthEdit, "Width has not been set.");
		checkInput(lengthEdit, "Length has not been set.");
		checkInput(heightEdit, "Height has not been set.");
		checkInput(spacingEdit, "Spacing has not been set.");
		checkInput(densityEdit, "Density has not been set.");
	}
	catch(SpikeStreamException& ex){
		QMessageBox::warning(this, "Cuboid Neuron Group Builder", ex.getMessage(), QMessageBox::Ok);
		return;
	}

	//Extract variables
	int xPos = Util::getInt(xPosEdit->text());
	int yPos = Util::getInt(yPosEdit->text());
	int zPos = Util::getInt(zPosEdit->text());
	int width = Util::getInt(widthEdit->text());
	int length = Util::getInt(lengthEdit->text());
	int height = Util::getInt(heightEdit->text());
	int spacing = Util::getInt(spacingEdit->text());
	double density = Util::getDouble(densityEdit->text());
	int neuronType = getNeuronTypeID(neuronTypeCombo->currentText());

	//Store parameters in parameter map
	QHash<QString, double> paramMap;
	paramMap["x"] = xPos;
	paramMap["y"] = yPos;
	paramMap["z"] = zPos;
	paramMap["width"] = width;
	paramMap["length"] = length;
	paramMap["height"] = height;
	paramMap["spacing"] = spacing;
	paramMap["density"] = density;

	/* Check that there are no conflicts with existing neurons
	   Neurons are added starting at (x,y,z) and finishing when x<x+width, y<y+length and z<z+height */
	Box neurGrpBox(xPos, yPos, zPos, xPos+width-1, yPos+length-1, zPos+height-1);
	unsigned int numOverlappingNeurons = Globals::getNetworkDao()->getNeuronCount(Globals::getNetwork()->getID(), neurGrpBox);
	if(numOverlappingNeurons != 0){
		QMessageBox::critical(this, "Cuboid Neuron Group Builder Error", "Proposed neuron group overlaps with a neuron group that is already in the database.\nPlease change the position and/or the width, length and height.", QMessageBox::Ok);
		return;
	}

	//Start thread to add neuron group
	NeuronGroupInfo info(0, nameEdit->text(), descriptionEdit->text(), paramMap, neuronType);
	builderThread->prepareAddNeuronGroup(info);
	progressDialog = new QProgressDialog("Building neuron group", "Cancel", 0, 100, this);
	progressDialog->setWindowModality(Qt::WindowModal);
	progressDialog->setMinimumDuration(2000);
	builderThread->start();
}


/*! Called when the builder thread has finished */
void CuboidWidget::builderThreadFinished(){
	if(builderThread->isError())
		qCritical()<<builderThread->getErrorMessage();

	//Inform other classes that network has changed
	Globals::getEventRouter()->networkChangedSlot();
}


/*! Updates user with feedback about progress with the operation */
void CuboidWidget::updateProgress(int stepsCompleted, int totalSteps){
	if(stepsCompleted < totalSteps){
		//Update progress
		progressDialog->setValue(stepsCompleted);
		progressDialog->setMaximum(totalSteps);

		//Check for cancellation
		if(progressDialog->wasCanceled()){
			builderThread->stop();
		}
	}
	else{
		progressDialog->close();
		//delete progressDialog;
	}
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds neuron types to the specified combo */
void CuboidWidget::addNeuronTypes(QComboBox* combo){
	QList<NeuronType> neuronTypesList = Globals::getNetworkDao()->getNeuronTypes();
	foreach(NeuronType neurType, neuronTypesList){
		combo->addItem(neurType.getDescription() + "(" + QString::number(neurType.getID()) + ")");
	}
}


/*! Checks that there is valid input in the specified combo box and throws an exception with the specified error if not. */
void CuboidWidget::checkInput(QLineEdit* inputEdit, const QString& errMsg){
	if(inputEdit->text().isEmpty()){
		throw SpikeStreamException(errMsg);
	}
}


/*! Extracts the neuron group ID from the text of a combo box */
unsigned int CuboidWidget::getNeuronTypeID(const QString& comboText){
	if(comboText.isEmpty())
		throw SpikeStreamException("Cannot extract a neuron type ID from empty text.");

	QRegExp regExp("[()]");
	qDebug()<<"TEST SECTION: "<<comboText.section(regExp, 1, 1);
	return Util::getUInt(comboText.section(regExp, 1, 1));
}

