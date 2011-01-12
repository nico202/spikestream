//SpikeStream includes
#include "RasterPlotDialog.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QLayout>


/*! Constructor */
RasterPlotDialog::RasterPlotDialog(QList<NeuronGroup*> neuronGroupList, QWidget* parent) : QDialog(parent){
	QVBoxLayout* mainVBox = new QVBoxLayout(this);

	try{
		//Create data
		rasterModel = new RasterModel(neuronGroupList);

		//Add view to display data
		rasterView = new RasterView(rasterModel, this);
		mainVBox->addWidget(rasterView);
	}
	catch(SpikeStreamException& ex){
		qCritical()<<"Raster error: "<<ex.getMessage();
	}
	catch(...){
		qCritical()<<"An unknown exception occurred";
	}
}


/*! Destructor */
RasterPlotDialog::~RasterPlotDialog(){
}


/*! Adds firing neuron data and replots the spike raster. */
void RasterPlotDialog::addData(const QList<unsigned>& firingNeuronIDs, unsigned timeStep){
	rasterModel->addData(firingNeuronIDs, timeStep);
	rasterView->replot();
}
