#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

//SpikeStream includes
#include "ConnectionGroupModel.h"

//Qt includes
#include <QProgressDialog>
#include <QPushButton>
#include <QWidget>

namespace spikestream {

	class ConnectionWidget : public QWidget {
		Q_OBJECT

		public:
			ConnectionWidget(QWidget* parent = 0);
			~ConnectionWidget();


		private slots:
			void addConnections();
			void deleteSelectedConnections();
			void networkChanged();
			void networkTaskFinished();

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

	};

}

#endif//CONNECTIONWIDGET_H



