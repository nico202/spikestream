//SpikeStream includes
#include "NemoParametersDialog.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>


/*! Constructor */
NemoParametersDialog::NemoParametersDialog(QWidget *parent) : QDialog(parent) {
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	mainVBox->addWidget(new QLabel("nemo param dialog"));
}


/*! Destructor */
NemoParametersDialog::~NemoParametersDialog(){
}
