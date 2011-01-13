//SpikeStream includes
#include "SpikeRasterDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLayout>


/*! Constructor */
SpikeRasterDialog::SpikeRasterDialog(QList<NeuronGroup*> neuronGroupList, QWidget* parent) : QDialog(parent){
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	try{
		//Create data
//		rasterModel = new RasterModel(neuronGroupList);

//		//Add view to display data
//		rasterView = new RasterView(rasterModel, this);
//		mainVBox->addWidget(rasterView);

		spikeRasterWidget = new SpikeRasterWidget(neuronGroupList, this);
		mainVBox->addWidget(spikeRasterWidget);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Raster error: "<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred";
	}
}


/*! Destructor */
SpikeRasterDialog::~SpikeRasterDialog(){
}


/*! Adds firing neuron data and replots the spike raster. */
void SpikeRasterDialog::addData(const QList<unsigned>& firingNeuronIDs, unsigned timeStep){
	spikeRasterWidget->addSpikes(firingNeuronIDs,timeStep);
}
