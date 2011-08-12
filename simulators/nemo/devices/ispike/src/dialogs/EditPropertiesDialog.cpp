//SpikeStream includes
#include "EditPropertiesDialog.h"
#include "Globals.h"
#include "Util.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//iSpike includes
#include "iSpike/ISpikeException.hpp"
using namespace ispike;

//Qt includes
#include <QDebug>
#include <QLabel>
#include <QPixmap>

#define NEURON_WIDTH_STRING "Neuron Width"
#define NEURON_HEIGHT_STRING "Neuron Height"

/*! Constructor used in standard mode. */
EditPropertiesDialog::EditPropertiesDialog(map<string, Property> propertyMap, bool disableReadOnly, QWidget* parent) : QDialog(parent) {
	//Store and initialize variables
	this->propertyMap = propertyMap;
	this->disableReadOnly = disableReadOnly;
	neuronGroup = NULL;
	neuronGroupSelectionMode = false;

	//Add properties to layout
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	addParameters(mainVBox);
	addButtons(mainVBox);
}


/*! Constructor used when selecting a neuron group */
EditPropertiesDialog::EditPropertiesDialog(map<string, Property> propertyMap, bool disableReadOnly, QList<NeuronGroup*> neuronGroupList, QWidget* parent) : QDialog(parent){
	//Store and initialize variables
	this->propertyMap = propertyMap;
	this->disableReadOnly = disableReadOnly;
	this->neuronGroupList = neuronGroupList;
	neuronGroup = NULL;
	neuronGroupSelectionMode = true;

	//Extract number of neurons, which is used in neuron group selection
	if(propertyMap.count(NEURON_WIDTH_STRING) == 0 || propertyMap.count(NEURON_HEIGHT_STRING) == 0)
		throw SpikeStreamException("Can only select neuron groups when parameters 'Neuron Width' and 'Neuron Height' are defined.");

	//Add properties to layout
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	addParameters(mainVBox);
	addNeuronGroups(mainVBox);
	addButtons(mainVBox);
}


/*! Destructor */
EditPropertiesDialog::~EditPropertiesDialog(){
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE SLOTS                  -----*/
/*----------------------------------------------------------*/

/*! Called when ok button is clicked. Extracts parameters from GUI and sets them in the map */
void EditPropertiesDialog::okButtonClicked(){
	try{
		storeParameterValues();
		accept();
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
	catch(ISpikeException& ex){
		qCritical()<<ex.what();
	}
}


/*! Updates neuron combo to reflect changes in the number of neurons property */
void EditPropertiesDialog::updateNeuronCombo(){
	if(!neuronGroupSelectionMode)
		return;

	int numNeurons = Util::getInt(lineEditMap[NEURON_WIDTH_STRING]->text()) * Util::getInt(lineEditMap[NEURON_HEIGHT_STRING]->text());
	updateCompatibleNeuronGroups(numNeurons);
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds cancel, load defaults, and ok button to the supplied layout. */
void EditPropertiesDialog::addButtons(QVBoxLayout* mainVLayout){
	QHBoxLayout *buttonBox = new QHBoxLayout();
	QPushButton* cancelButton = new QPushButton("Cancel");
	buttonBox->addWidget(cancelButton);
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QPushButton* okButton = new QPushButton("Ok");
	buttonBox->addWidget(okButton);
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
	mainVLayout->addLayout(buttonBox);
	okButton->setFocus(Qt::OtherFocusReason);
}


/*! Adds combo for selecting neuron groups */
void EditPropertiesDialog::addNeuronGroups(QVBoxLayout* mainVBox){
	QHBoxLayout* neurGrpBox = new QHBoxLayout();
	neurGrpBox->addWidget(new QLabel("Neuron Group: "));
	neuronGroupCombo = new QComboBox();
	neurGrpBox->addWidget(neuronGroupCombo);
	mainVBox->addLayout(neurGrpBox);

	//Load the neuron groups that are compatible with this number of neurons
	int numberOfNeurons = propertyMap[NEURON_WIDTH_STRING].getInt() * propertyMap[NEURON_HEIGHT_STRING].getInt();
	updateCompatibleNeuronGroups(numberOfNeurons);
}


/*! Adds the parameters to the layout with tool tips. */
void EditPropertiesDialog::addParameters(QVBoxLayout* mainVLayout){
	int cntr = 0;
	QGridLayout* gridLayout = new QGridLayout();

	//Create validators
	QDoubleValidator* doubleValidator = new QDoubleValidator(-100000.0, 100000.0, 5, this);
	QIntValidator* intValidator = new QIntValidator(-1000000, 1000000, this);

	//Add parameters to the layout
	for(map<string, Property>::iterator iter = propertyMap.begin(); iter != propertyMap.end(); ++iter){
		QString propertyName(iter->second.getName().data());

		//Add double parameter
		if(iter->second.getType() == Property::Double){
			gridLayout->addWidget(new QLabel(propertyName), cntr, 0);
			QLineEdit* tmpLineEdit = new QLineEdit( QString::number(iter->second.getDouble() ) );
			tmpLineEdit->setValidator(doubleValidator);
			lineEditMap[propertyName] = tmpLineEdit;
			gridLayout->addWidget(tmpLineEdit, cntr, 1);

			//Disable read only properties
			if (disableReadOnly && iter->second.isReadOnly())
				lineEditMap[propertyName]->setEnabled(false);
		}

		//Add integer parameter
		else if(iter->second.getType() == Property::Integer){
			gridLayout->addWidget(new QLabel(propertyName), cntr, 0);
			QLineEdit* tmpLineEdit = new QLineEdit( QString::number( iter->second.getInt() ) );
			tmpLineEdit->setValidator(intValidator);
			lineEditMap[propertyName] = tmpLineEdit;
			gridLayout->addWidget(tmpLineEdit, cntr, 1);

			//Disable read only properties
			if (disableReadOnly && iter->second.isReadOnly())
				lineEditMap[propertyName]->setEnabled(false);

			//Listen for changes in the number of neurons
			if(neuronGroupSelectionMode && (propertyName == NEURON_WIDTH_STRING || propertyName == NEURON_HEIGHT_STRING) )
				connect(tmpLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateNeuronCombo()));
		}

		//Add string parameter
		else if(iter->second.getType() == Property::String){
			gridLayout->addWidget(new QLabel(propertyName), cntr, 0);
			QLineEdit* tmpLineEdit = new QLineEdit( QString( iter->second.getString().data() ) );
			lineEditMap[propertyName] = tmpLineEdit;
			gridLayout->addWidget(tmpLineEdit, cntr, 1);

			//Disable read only properties
			if (disableReadOnly && iter->second.isReadOnly())
				lineEditMap[propertyName]->setEnabled(false);
		}

		//Add combo parameter
		else if(iter->second.getType() == Property::Combo){
			gridLayout->addWidget(new QLabel(propertyName), cntr, 0);

			//Build and add combo
			QComboBox* tmpCombo = new QComboBox();
			int tmpIndex = 0;
			vector<string> comboOptions = iter->second.getOptions();
			for(size_t i=0; i<comboOptions.size(); ++i){
				tmpCombo->addItem(QString(comboOptions[i].data()));
				if(comboOptions[i] == iter->second.getString())
					tmpIndex = i;
			}
			comboMap[propertyName] = tmpCombo;
			tmpCombo->setCurrentIndex(tmpIndex);
			gridLayout->addWidget(tmpCombo, cntr, 1);

			//Disable read only properties
			if (disableReadOnly && iter->second.isReadOnly())
				comboMap[propertyName]->setEnabled(false);
		}

		//Unknown property type
		else{
			throw SpikeStreamException("Parameter type not recognized: " + QString::number(iter->second.getType()));
		}

		//Add help tool tip
		QLabel* tmpLabel = new QLabel();
		tmpLabel->setPixmap(QPixmap(Globals::getSpikeStreamRoot() + "/images/help.png"));
		tmpLabel->setToolTip(QString(iter->second.getDescription().data()));
		gridLayout->addWidget(tmpLabel, cntr, 2);
		++cntr;
	}
	mainVLayout->addLayout(gridLayout);
}


/*! Returns a standard formatted neuron group name including the ID. */
QString EditPropertiesDialog::getNeuronGroupName(NeuronGroup* neuronGroup){
	return neuronGroup->getInfo().getName() + " (" + QString::number(neuronGroup->getID()) + ")";
}


/*! Stores the properties */
void EditPropertiesDialog::storeParameterValues(){
	//Run some basic checks
	if( (lineEditMap.size() + comboMap.size()) != (int)propertyMap.size())
		throw SpikeStreamException("Property map size does not match list of parameters.");

	//Work through the properties
	for(map<string, Property>::iterator iter = propertyMap.begin(); iter != propertyMap.end(); ++iter){
		QString propertyName(iter->second.getName().data());

		//Store double parameter
		if(iter->second.getType() == Property::Double){
			//Run some checks
			if(!lineEditMap.contains(propertyName))
				throw SpikeStreamException("Property " + propertyName + " cannot be found.");
			if(lineEditMap[propertyName]->text().isEmpty())
				throw SpikeStreamException("Property " + propertyName + " must be a double.");

			//Store value
			iter->second.setDouble( Util::getDouble(lineEditMap[propertyName]->text()) );
		}

		//Store integer parameter
		else if(iter->second.getType() == Property::Integer){
			//Run some checks
			if(!lineEditMap.contains(propertyName))
				throw SpikeStreamException("Property " + propertyName + " cannot be found.");
			if(lineEditMap[propertyName]->text().isEmpty())
				throw SpikeStreamException("Property " + propertyName + " must be an integer.");

			//Store value
			iter->second.setInt( Util::getInt(lineEditMap[propertyName]->text()) );
		}

		//Store string parameter
		else if(iter->second.getType() == Property::String){
			//Run some checks
			if(!lineEditMap.contains(propertyName))
				throw SpikeStreamException("Property " + propertyName + " cannot be found.");
			if(lineEditMap[propertyName]->text().isEmpty())
				throw SpikeStreamException("Property " + propertyName + " is an empty string.");

			//Store value
			iter->second.setString( lineEditMap[propertyName]->text().toStdString() );
		}

		//Store combo parameter
		else if(iter->second.getType() == Property::Combo){
			//Run some checks
			if(!comboMap.contains(propertyName))
				throw SpikeStreamException("Property " + propertyName + " cannot be found.");

			//Store value
			iter->second.setString( comboMap[propertyName]->currentText().toStdString() );
		}

		//Unknown property type
		else{
			throw SpikeStreamException("Property type not recognized: " + QString::number(iter->second.getType()));
		}
	}

	//Store neuron group if required
	if(neuronGroupSelectionMode){
		//Work through list to find group
		neuronGroup = NULL;
		foreach(NeuronGroup* tmpNeurGrp, neuronGroupList){
			if(getNeuronGroupName(tmpNeurGrp) == neuronGroupCombo->currentText())
				neuronGroup = tmpNeurGrp;
		}

		//Check group was found
		if(neuronGroup == NULL)
			throw SpikeStreamException("Neuron group has not been selected.");
	}
}


/*! Adds neuron groups of the correct size to the neuron group combo. */
void EditPropertiesDialog::updateCompatibleNeuronGroups(int numberOfNeurons){
	neuronGroupCombo->clear();
	foreach(NeuronGroup* tmpNeurGrp, neuronGroupList){
		if(tmpNeurGrp->size() == numberOfNeurons){
			neuronGroupCombo->addItem(getNeuronGroupName(tmpNeurGrp));
		}
	}
}


