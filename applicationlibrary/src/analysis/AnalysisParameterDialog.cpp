//SpikeStream includes
#include "NumberConversionException.h"
#include "AnalysisParameterDialog.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QDoubleValidator>
#include <QLayout>
#include <QLabel>
#include <QPushButton>


/*! Constructor */
AnalysisParameterDialog::AnalysisParameterDialog(QWidget* parent, const AnalysisInfo &analysisInfo) : QDialog(parent){
	//Store variables
	this->info = analysisInfo;

	//Create layouts
	QVBoxLayout* mainVerticalBox = new QVBoxLayout(this);
	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(10);

	//Validators for double and integer parameters
	QDoubleValidator* doubleValidator = new QDoubleValidator(this);
	doubleValidator->setDecimals(5);
	QIntValidator* intValidator = new QIntValidator(0, 50, this);

	//Add the description
	gridLayout->addWidget(new QLabel("Analysis description: "), 0, 0);
	descriptionEdit = new QLineEdit(info.getDescription());
	gridLayout->addWidget(descriptionEdit, 0, 1);

	//Add the number of threads
	gridLayout->addWidget(new QLabel("Number of simultaneous threads: "), 1, 0);
	numThreadsEdit = new QLineEdit(QString::number(info.getNumberOfThreads()));
	numThreadsEdit->setValidator(intValidator);
	gridLayout->addWidget(numThreadsEdit, 1, 1);

	//Add the parameters
	int cntr = 2;
	for(QHash<QString, double>::iterator iter = info.getParameterMap().begin(); iter != info.getParameterMap().end(); ++iter){
		gridLayout->addWidget(new QLabel(iter.key()), cntr, 0);
		QLineEdit* tmpEdit = new QLineEdit(QString::number(iter.value(), 'g', 10));//'g' sets the output similar to sprintf; 10 is the maximum precision
		tmpEdit->setValidator(doubleValidator);
		//Disable parameter editing if it is loaded from the database
		if(analysisInfo.getID() != 0){
			tmpEdit->setEnabled(false);
		}
		gridLayout->addWidget(tmpEdit, cntr, 1);
		editMap[iter.key()] = tmpEdit;
		++cntr;
	}
	mainVerticalBox->addLayout(gridLayout);

	//Add Ok and Cancel buttons
	QHBoxLayout *okCanButtonBox = new QHBoxLayout();
	QPushButton *okPushButton = new QPushButton("Ok");
	connect(okPushButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
	QPushButton *cancelPushButton = new QPushButton("Cancel");
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));
	okCanButtonBox->addWidget(okPushButton);
	okCanButtonBox->addWidget(cancelPushButton);
	mainVerticalBox->addLayout(okCanButtonBox);

}


/*! Destructor */
AnalysisParameterDialog::~AnalysisParameterDialog(){
}


/*----------------------------------------------------------*/
/*-----                  PUBLIC METHODS                -----*/
/*----------------------------------------------------------*/

/*! Returns the analysis info with parameters and description set by the dialog */
const AnalysisInfo& AnalysisParameterDialog::getInfo(){
	return info;
}


/*----------------------------------------------------------*/
/*-----                  PRIVATE SLOTS                 -----*/
/*----------------------------------------------------------*/

/*! Checks that inputs are correct and adds them to the analysis info */
void AnalysisParameterDialog::okButtonClicked(){
	//Add the description
	if(descriptionEdit->text() == "")
		info.setDescription("Undescribed");
	else
		info.setDescription(descriptionEdit->text());

	//Number of simultaneous threads
	if(numThreadsEdit->text() == ""){
		qCritical()<<"Number of threads has not been set.";
		return;
	}

	//Load the current parameters into the analysis info
	try{
		info.setNumberOfThreads( Util::getUInt(numThreadsEdit->text()) );

		for(QHash<QString, QLineEdit*>::iterator iter = editMap.begin(); iter != editMap.end(); ++iter){
			info.getParameterMap()[iter.key()] = Util::getDouble(iter.value()->text());
		}
	}
	catch(NumberConversionException& ex){
		//Reset map and display error message
		info.getParameterMap().clear();
		qCritical()<<ex.getMessage();
		return;
	}

	//If we have reached this point, info should be ok
	accept();
}


