#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

//SpikeStream includes
#include "ConnectionGroupModel.h"
#include "ConnectionManager.h"

//Qt includes
#include <QMutex>
#include <QProgressDialog>
#include <QPushButton>
#include <QWidget>

namespace spikestream {

	/*! Displays a table listing the connection groups in the current network and tools to add,
		delete, show and hide connection groups. */
	class ConnectionWidget : public QWidget {
		Q_OBJECT

		public:
			ConnectionWidget(QWidget* parent = 0);
			~ConnectionWidget();


		private slots:
			void addConnections();
			void connectionManagerFinished();
			void deleteSelectedConnections();
			void networkChanged();
			void updateProgress(int stepsCompleted, int totalSteps, QString message);

		private:
			//======================  VARIABLES  =========================
			/*! Model corresponding to the view */
			ConnectionGroupModel* connectionGroupModel;

			/*! Button for adding connection groups */
			QPushButton* addConnectionsButton;

			/*! Button for deleting connection groups */
			QPushButton* deleteButton;

			/*! Provides feedback with progress deleting neurons */
			QProgressDialog* progressDialog;

			/*! Separate thread to delete connection groups */
			ConnectionManager* connectionManager;

			/*! Flag to prevent calls to progress dialog while it is redrawing. */
			bool updatingProgress;

	};

}

#endif//CONNECTIONWIDGET_H



