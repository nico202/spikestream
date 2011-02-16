//SpikeStream includes
#include "MembranePotentialGraphDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLayout>


/*! Constructor */
MembranePotentialGraphDialog::MembranePotentialGraphDialog(neurid_t neuronID, QWidget* parent) : QDialog(parent){
	QVBoxLayout* mainVBox = new QVBoxLayout(this);
	this->setWindowTitle("Membrane Potential for Neuron " + QString::number(neuronID));

	try{
		memPotWidget = new MembranePotentialGraphWidget(this);
		mainVBox->addWidget(memPotWidget);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Membrane potential plot error: "<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred";
	}
}


/*! Destructor */
MembranePotentialGraphDialog::~MembranePotentialGraphDialog(){
}


/*----------------------------------------------------------*/
/*------                PRIVATE SLOTS                 ------*/
/*----------------------------------------------------------*/

/*! Adds membrane potential data and replots the graph. */
void MembranePotentialGraphDialog::addData(float membranePotential, timestep_t timeStep){
	memPotWidget->addData(membranePotential,timeStep);
}
