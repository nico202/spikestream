//SpikeStream includes
#include "Globals.h"
#include "LivelinessExportDialog.h"
#include "LivelinessDao.h"
using namespace spikestream;


/*! Constructor */
LivelinessExportDialog::LivelinessExportDialog(QWidget* parent) : AbstractExportAnalysisDialog(parent){
}


/*! Destructor */
LivelinessExportDialog::~LivelinessExportDialog(){
}


/*! Exports the analysis results as a comma separated file */
void LivelinessExportDialog::exportCommaSeparated(const QString& filePath){
	//Get the data to be exported
	LivelinessDao livelinessDao(Globals::getAnalysisDao()->getDBInfo());
	QList<Cluster> clusterList = livelinessDao.getClusters(Globals::getAnalysisID());

	//Open the file
	QFile outFile(filePath);
	if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text)){
		qCritical()<<"File: "<<filePath<<" could not be opened.";
		return;
	}
	QTextStream outStream(&outFile);

	//Write headers for cluster information
	outStream<<"Analysis ID\tTime Step\tLiveliness\tNeuron IDs\n";

	//Write complexes
	foreach(Cluster tmpCluster, clusterList){
		outStream<<tmpCluster.getID()<<"\t";
		outStream<<tmpCluster.getTimeStep()<<"\t";
		outStream<<tmpCluster.getLiveliness()<<"\t";
		outStream<<tmpCluster.getNeuronIDString()<<"\n";
	}

	//FIXME
//	//Write headers for neuron liveliness information
//	outStream<<"Neuron ID\tTime Step\tLiveliness\n";
//
//	//Write complexes
//	foreach(Cluster tmpCluster, clusterList){
//		outStream<<tmpCluster.getID()<<"\t";
//		outStream<<tmpCluster.getTimeStep()<<"\t";
//		outStream<<tmpCluster.getLiveliness()<<"\t";
//		outStream<<tmpCluster.getNeuronIDString()<<"\n";
//	}


	//Close the file
	outFile.close();
}

