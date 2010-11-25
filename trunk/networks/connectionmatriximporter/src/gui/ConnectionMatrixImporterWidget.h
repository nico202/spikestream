#ifndef CONNECTIONMATRIXIMPORTERWIDGET_H
#define CONNECTIONMATRIXIMPORTERWIDGET_H

//SpikeStream includes
#include "MatrixImporter.h"
#include "ParameterInfo.h"

//Qt includes
#include <QHash>
#include <QLineEdit>
#include <QProgressDialog>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace spikestream {

	/*! Graphical interface used to control the import of a connection matrix and associated
		components. */
	class ConnectionMatrixImporterWidget : public QWidget {
		Q_OBJECT

		public:
			ConnectionMatrixImporterWidget();
			~ConnectionMatrixImporterWidget();

		private slots:
			void checkImportEnabled(const QString& = "");
			void getCoordinatesFile();
			void getDelaysFile();
			void getNodeNamesFile();
			void getWeightsFile();
			void importMatrix();
			void matrixImporterFinished();
			void updateProgress(int stepsCompleted, int totalSteps, QString message);
			void setParameters();

		private:
			//======================  VARIABLES  =======================
			/*! Name of network that is imported. */
			QLineEdit* networkNameEdit;

			/*! Path to file containing coordinate data. */
			QLineEdit* coordinatesFilePathEdit;

			/*! Path to file containing node names */
			QLineEdit* nodeNamesFilePathEdit;

			/*! Path to file containing connectivity matrix with weights */
			QLineEdit* weightsFilePathEdit;

			/*! Path to file containing delays */
			QLineEdit* delaysFilePathEdit;

			/*! Button that starts import process. */
			QPushButton* importButton;

			/*! Default values of parameters */
			QHash<QString, double> defaultParameterMap;

			/*! Current values of parameters */
			QHash<QString, double> parameterMap;

			/*! List of parameters for this class */
			QList<ParameterInfo> parameterInfoList;

			/*! Class that carries out the importing. */
			MatrixImporter* matrixImporter;

			/*! Dialog for showing progress */
			QProgressDialog* progressDialog;

			/*! Avoids multiple calls to progress dialog whilst it is redrawing */
			bool progressUpdating;

			/*! Network that is being built. */
			Network* newNetwork;


			//======================  METHODS  =========================
			void buildParameters();
			QString getFilePath(QString fileFilter);
    };

}

#endif//CONNECTIONMATRIXIMPORTERWIDGET_H

