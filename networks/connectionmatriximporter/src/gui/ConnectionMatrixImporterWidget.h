#ifndef CONNECTIONMATRIXIMPORTERWIDGET_H
#define CONNECTIONMATRIXIMPORTERWIDGET_H

//SpikeStream includes
//#include "NeuronGroupBuilder.h"

//Qt includes
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace spikestream {

	class ConnectionMatrixImporterWidget : public QWidget {
		Q_OBJECT

		public:
			ConnectionMatrixImporterWidget();
			~ConnectionMatrixImporterWidget();

		private slots:
			void checkImportEnabled(const QString& = "");
			void getCoordinatesFile();
			void importMatrix();

		private:
			//======================  VARIABLES  =======================
			/*! Name of network that is imported. */
			QLineEdit* networkName;

			/*! Path to file containing coordinate data. */
			QLineEdit* coordinatesFilePath;

			/*! Button that starts import process. */
			QPushButton* importButton;


			//======================  METHODS  =========================
			QString getFilePath(QString fileFilter);
    };

}

#endif//CONNECTIONMATRIXIMPORTERWIDGET_H

