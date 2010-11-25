//SpikeStream includes
#include "AbstractParametersEditDialog.h"
#include "Globals.h"
#include "Util.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QPixmap>


/*! Constructor */
AbstractParametersEditDialog::AbstractParametersEditDialog(const QList<ParameterInfo>& paramInfoList, QWidget* parent) : QDialog(parent) {
	this->parameterInfoList = paramInfoList;
}


/*! Destructor */
AbstractParametersEditDialog::~AbstractParametersEditDialog(){
}


/*----------------------------------------------------------*/
/*-----               PROTECTED METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds the parameters to the layout with tool tips */
void AbstractParametersEditDialog::addParameters(QVBoxLayout* mainVLayout){
	int cntr = 0;
	QGridLayout* gridLayout = new QGridLayout();

	//Create validators
	QDoubleValidator* doubleValidator = new QDoubleValidator(-100000.0, 100000.0, 5, this);
	QDoubleValidator* positiveDoubleValidator = new QDoubleValidator(0.0, 100000.0, 5, this);
	QIntValidator* intValidator = new QIntValidator(-1000000, 1000000, this);
	QIntValidator* unsignedIntValidator = new QIntValidator(0, 1000000, this);

	//Add parameters to the layout
	foreach(ParameterInfo info, parameterInfoList){
		//Add double parameter
		if(info.getType() == ParameterInfo::DOUBLE){
			gridLayout->addWidget(new QLabel(info.getName()), cntr, 0);
			QLineEdit* tmpLineEdit = new QLineEdit();
			tmpLineEdit->setValidator(doubleValidator);
			lineEditMap[info.getName()] = tmpLineEdit;
			gridLayout->addWidget(tmpLineEdit, cntr, 1);
		}

		//Add positive double parameter
		else if(info.getType() == ParameterInfo::POSITIVE_DOUBLE){
			gridLayout->addWidget(new QLabel(info.getName()), cntr, 0);
			QLineEdit* tmpLineEdit = new QLineEdit();
			tmpLineEdit->setValidator(positiveDoubleValidator);
			lineEditMap[info.getName()] = tmpLineEdit;
			gridLayout->addWidget(tmpLineEdit, cntr, 1);
		}

		//Add integer parameter
		else if(info.getType() == ParameterInfo::INTEGER){
			gridLayout->addWidget(new QLabel(info.getName()), cntr, 0);
			QLineEdit* tmpLineEdit = new QLineEdit();
			tmpLineEdit->setValidator(intValidator);
			lineEditMap[info.getName()] = tmpLineEdit;
			gridLayout->addWidget(tmpLineEdit, cntr, 1);
		}

		//Add unsigned integer parameter
		else if(info.getType() == ParameterInfo::UNSIGNED_INTEGER){
			gridLayout->addWidget(new QLabel(info.getName()), cntr, 0);
			QLineEdit* tmpLineEdit = new QLineEdit();
			tmpLineEdit->setValidator(unsignedIntValidator);
			lineEditMap[info.getName()] = tmpLineEdit;
			gridLayout->addWidget(tmpLineEdit, cntr, 1);
		}

		//Add boolean parameter
		else if(info.getType() == ParameterInfo::BOOLEAN){
			QCheckBox* chkBox = new QCheckBox(info.getName());
			checkBoxMap[info.getName()] = chkBox;
			gridLayout->addWidget(chkBox, cntr, 0);
		}

		//Add option parameter - a list in a combo box
		else if(info.getType() == ParameterInfo::OPTION){
			gridLayout->addWidget(new QLabel(info.getName()), cntr, 0);
			QComboBox* tmpComboBox = new QComboBox();
			foreach(QString optionName, info.getOptionNames())
				tmpComboBox->addItem(optionName);
			comboMap[info.getName()] = tmpComboBox;
			gridLayout->addWidget(tmpComboBox, cntr, 1);
		}

		//Unknown parameter type
		else{
			throw SpikeStreamException("Parameter type not recognized: " + info.getType());
		}

		//Add help tool tip
		QLabel* tmpLabel = new QLabel();
		tmpLabel->setPixmap(QPixmap(Globals::getSpikeStreamRoot() + "/images/help.png"));
		tmpLabel->setToolTip(info.getDescription());
		gridLayout->addWidget(tmpLabel, cntr, 2);
		++cntr;
	}
	mainVLayout->addLayout(gridLayout);
}


/*! Adds cancel, load defaults, and ok button to the supplied layout. */
void AbstractParametersEditDialog::addButtons(QVBoxLayout* mainVLayout){
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


/*! Returns a map with the parameter values that have been entered by the user */
QHash<QString, double> AbstractParametersEditDialog::getParameterValues(){
	QHash<QString, double> paramMap;

	//Extract double, int and unsigned int parameters
	for(QHash<QString, QLineEdit*>::iterator iter = lineEditMap.begin(); iter != lineEditMap.end(); ++iter){
		QString paramStr = iter.value()->text();
		if(paramStr.isEmpty())
			throw SpikeStreamException(iter.key() + " has not been entered.");

		paramMap[iter.key()] = Util::getDouble(paramStr);
	}

	//Extract boolean parameters - store as 1 or 0 in parameter map
	for(QHash<QString, QCheckBox*>::iterator iter = checkBoxMap.begin(); iter != checkBoxMap.end(); ++iter){
		if(iter.value()->isChecked())
			paramMap[iter.key()] = 1.0;
		else
			paramMap[iter.key()] = 0.0;
	}

	//Extract option parameters - store the option index in the parameter map
	for(QHash<QString, QComboBox*>::iterator iter = comboMap.begin(); iter != comboMap.end(); ++iter){
		paramMap[iter.key()] = iter.value()->currentIndex();
	}

	//Check all parameters have been extracted
	if(paramMap.size() != parameterInfoList.size()){
		throw SpikeStreamException("Failed to find all parameters in list or map has too many entries.");
	}

	//Return populated map
	return paramMap;
}


/*! Sets the values in the text edits to the values stored in the map */
void AbstractParametersEditDialog::setParameterValues(const QHash<QString, double>& paramMap){
	//Run some basic checks
	if(paramMap.size() != parameterInfoList.size())
		throw SpikeStreamException("Parameter map size does not match list of parameters.");
	if(paramMap.size() != ( lineEditMap.size() + checkBoxMap.size() + comboMap.size() ) )
		throw SpikeStreamException("Mismatch between the parameter map and the stored parameters.");

	//Set values in the line edits
	for(QHash<QString, QLineEdit*>::iterator iter = lineEditMap.begin(); iter != lineEditMap.end(); ++iter){
		if( !paramMap.contains(iter.key()) )
			throw SpikeStreamException("A value for parameter " + iter.key() + " cannot be found in the parameter map.");
		iter.value()->setText(QString::number(paramMap[iter.key()]));
	}

	//Set check box values
	for(QHash<QString, QCheckBox*>::iterator iter = checkBoxMap.begin(); iter != checkBoxMap.end(); ++iter){
		if( !paramMap.contains(iter.key()) )
			throw SpikeStreamException("A value for parameter " + iter.key() + " cannot be found in the parameter map.");
		if(paramMap[iter.key()] == 0)
			iter.value()->setChecked(false);
		else
			iter.value()->setChecked(true);
	}

	//Set option values
	for(QHash<QString, QComboBox*>::iterator iter = comboMap.begin(); iter != comboMap.end(); ++iter){
		if( !paramMap.contains(iter.key()) )
			throw SpikeStreamException("A value for parameter " + iter.key() + " cannot be found in the parameter map.");

		//Check that the index is valid
		int index = (int)paramMap[iter.key()];
		if(index < 0 || index >= iter.value()->count())
			throw SpikeStreamException("Index out of range for combo box: " + QString::number(index));

		//Set the combo to the current index
		iter.value()->setCurrentIndex(index);
	}
}

