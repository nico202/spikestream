//SpikeStream includes
#include "Globals.h"
#include "StateBasedPhiExportDialog.h"
#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

/*! Constructor */
StateBasedPhiExportDialog::StateBasedPhiExportDialog(QWidget* parent) : AbstractExportAnalysisDialog(parent){
}


/*! Destructor */
StateBasedPhiExportDialog::~StateBasedPhiExportDialog(){
}


/*! Exports the analysis results as a comma separated file */
void StateBasedPhiExportDialog::exportCommaSeparated(const QString& filePath){
	//Get the data to be exported
	StateBasedPhiAnalysisDao stateDao(Globals::getAnalysisDao()->getDBInfo());
	QList<Complex> complexList = stateDao.getComplexes(Globals::getAnalysisID());

	//Open the file
	QFile outFile(filePath);
	if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text)){
		qCritical()<<"File: "<<filePath<<" could not be opened.";
		return;
	}
	QTextStream outStream(&outFile);

	//Write headers
	outStream<<"Analysis ID\tTime Step\tPhi\tNeuron IDs\n";

	//Write complexes
	foreach(Complex tmpCmplx, complexList){
		outStream<<tmpCmplx.getID()<<"\t";
		outStream<<tmpCmplx.getTimeStep()<<"\t";
		outStream<<tmpCmplx.getPhi()<<"\t";
		outStream<<tmpCmplx.getNeuronIDString()<<"\n";
	}

	//Close the file
	outFile.close();
}

