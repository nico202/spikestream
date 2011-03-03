//SpikeStream includes
#include "ISpikeWidget.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QLabel>


//Functions for dynamic library loading
extern "C" {
	/*! Creates an QWidget class when library is dynamically loaded. */
	QWidget* getClass(){
		return new ISpikeWidget();
	}

	/*! Returns a descriptive name for this widget */
	QString getName(){
		return QString("iSpike");
	}
}


/*! Constructor */
ISpikeWidget::ISpikeWidget(QWidget* parent) : AbstractDeviceWidget(parent){
	//Create layout for the entire widget
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	mainVBox->addWidget(new QLabel("iSpike WIDGET TEST LABEL"));

	//Create iSpike manager
	iSpikeManager = new ISpikeManager();
}


/*! Destructor */
ISpikeWidget::~ISpikeWidget(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Returns a pointer to the device manager */
AbstractDeviceManager* ISpikeWidget::getDeviceManager(){
	return (AbstractDeviceManager*)iSpikeManager;
}
