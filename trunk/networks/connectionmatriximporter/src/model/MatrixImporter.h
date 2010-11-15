#ifndef MATRIXIMPORTER_H
#define MATRIXIMPORTER_H

//SpikeStream includes
#include "SpikeStreamThread.h"

//Qt includes
#include <QHash>


namespace spikestream {

	/*! Thread that carries out the heavy tasks associated with matrix importing. */
	class MatrixImporter : public SpikeStreamThread {
		Q_OBJECT

		public:
			MatrixImporter();
			~MatrixImporter();
			void run();
			void startImport(const QString& netName, const QString& netDesc, const QString& coordinatesFilePath, const QString& nodeNamesFilePath, const QString& weightsFilePath, const QString& delaysFilePath, QHash<QString, double>& parameterMap);

		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);

		public slots:
			void updateProgress(int stepsCompleted, int totalSteps, QString message);


		private:
			//======================  VARIABLES  =======================
			/*! File holding the coordinates of the neuron groups */
			QString coordinatesFilePath;

			/*! Location of the file containing the delays of the connections between each neuron. */
			QString delaysFilePath;

			/*! Location of the file containing the list of node names. */
			QString nodeNamesFilePath;

			/*! Location of the file containing the list of node names. */
			QString weightsFilePath;

			/*! Name of the network to be created. */
			QString networkName;

			/*! Description of the network to be created */
			QString networkDescription;

			/*! Map containing parameters for the import */
			QHash<QString, double> parameterMap;


			//======================  METHODS  ======================
			void storeParameters(QHash<QString, double>& paramMap);
	};

}

#endif//MATRIXIMPORTER_H
