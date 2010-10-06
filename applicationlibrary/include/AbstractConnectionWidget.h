#ifndef ABSTRACTCONNECTIONWIDGET_H
#define ABSTRACTCONNECTIONWIDGET_H

//SpikeStream includes
#include "AbstractConnectionBuilder.h"
#include "ConnectionGroupInfo.h"

//Qt includes
#include <QComboBox>
#include <QLayout>
#include <QLineEdit>
#include <QProgressDialog>
#include <QWidget>

namespace spikestream{

	/*! Abstract widget implementing functionality common to all connection widget plugins. */
	class AbstractConnectionWidget : public QWidget {
		Q_OBJECT

		public:
			AbstractConnectionWidget(QWidget* parent = 0);
			virtual ~AbstractConnectionWidget();


		protected:
			//====================  VARIABLES  ===================
			/*! Class that runs as a separate thread to create connection group */
			AbstractConnectionBuilder* connectionBuilder;


			//====================  METHODS  =====================
			void addNeuronGroups(QComboBox* combo);
			void addSynapseTypes(QComboBox* combo);

			/*! Checks that the inputs to the plugin have sensible values. */
			virtual bool checkInputs() = 0;

			void checkInput(QComboBox* combo, const QString& errorMessage);
			void checkInput(QLineEdit* inputEdit, const QString& errorMessage);

			/*! Returns a connection group info specifying the network to be created. */
			virtual ConnectionGroupInfo getConnectionGroupInfo() = 0;

			/*! Creates a new connection builder.
				NOTE: should only be called once. */
			unsigned int getNeuronGroupID(const QString& comboText);
			unsigned int getSynapseTypeID(const QString& comboText);


		private slots:
			virtual void addButtonClicked();
			virtual void builderThreadFinished();
			virtual void updateProgress(int stepsCompleted, int totalSteps, QString message);

		private:
			//====================  VARIABLES  ======================
			/*! Displays progress with the operation */
			QProgressDialog* progressDialog;

			/*! Flag to prevent calls to progress dialog while it is redrawing. */
			bool updatingProgress;

	};
}

#endif//ABSTRACTCONNECTIONWIDGET_H
