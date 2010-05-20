//SpikeStream includes
#include "AbstractParametersEditDialog.h"
#include "Globals.h"
#include "Util.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
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

	//Create validator
	QDoubleValidator* doubleValidator = new QDoubleValidator(-100000.0, 100000.0, 5, this);

	//Add parameters to the layout
	foreach(ParameterInfo info, parameterInfoList){
		gridLayout->addWidget(new QLabel(info.getName()), cntr, 0);
		QLineEdit* tmpLineEdit = new QLineEdit();
		tmpLineEdit->setValidator(doubleValidator);
		lineEditMap[info.getName()] = tmpLineEdit;
		gridLayout->addWidget(tmpLineEdit, cntr, 1);
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
	for(QHash<QString, QLineEdit*>::iterator iter = lineEditMap.begin(); iter != lineEditMap.end(); ++iter){
		QString paramStr = iter.value()->text();
		if(paramStr.isEmpty())
			throw SpikeStreamException(iter.key() + " has not been entered.");

		paramMap[iter.key()] = Util::getDouble(paramStr);
	}
	return paramMap;
}


/*! Sets the values in the text edits to the values stored in the map */
void AbstractParametersEditDialog::setParameterValues(const QHash<QString, double>& paramMap){
	//Run some basic checks
	if(paramMap.size() != parameterInfoList.size())
		throw SpikeStreamException("Parameter map size does not match list of parameters.");

	//Set values in the line edits
	for(QHash<QString, QLineEdit*>::iterator iter = lineEditMap.begin(); iter != lineEditMap.end(); ++iter){
		if( !paramMap.contains(iter.key()) )
			throw SpikeStreamException("A value for parameter " + iter.key() + " cannot be found in the parameter map.");

		iter.value()->setText(QString::number(paramMap[iter.key()]));
	}
}

