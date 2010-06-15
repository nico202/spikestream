#ifndef CONNECTIONSMODEL_H
#define CONNECTIONSMODEL_H

//SpikeStream includes
#include "Connection.h"
using namespace spikestream;

//Qt includes
#include <QAbstractTableModel>
#include <QList>

namespace spikestream {

	/*! Model for displaying connection groups. Based on the ConnectionGroups table in the
		SpikeStreamNetwork database. */
	class ConnectionsModel : public QAbstractTableModel {
		Q_OBJECT

		public:
			ConnectionsModel();
			~ConnectionsModel();
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			int rowCount(const QModelIndex& parent = QModelIndex()) const;

		private slots:
			void networkDisplayChanged();

		private:
			//====================  VARIABLES  ====================
			/*! List containing the information about the neuron groups in the current network
			This list is automatically refreshed when the network changes. */
			QList<Connection*> connectionsList;

			static const int numCols = 7;
			static const int idCol = 0;
			static const int conGrpIDCol = 1;
			static const int fromIDCol = 2;
			static const int toIDCol = 3;
			static const int delayCol = 4;
			static const int weightCol = 5;
			static const int tmpWeightCol = 6;

			//=====================  METHODS  ======================
			void clearConnectionsList();
    };

}

#endif//CONNECTIONSMODEL_H

