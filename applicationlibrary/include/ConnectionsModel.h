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
		SpikeStreamNetwork database. Uses the connections list that is held in the
		NetworkDisplay class and populated by the NetworkViewer when it renders the connections. */
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
			void visibleConnectionsChanged();

		private:
			//====================  VARIABLES  ====================
			static const int numCols = 6;
			static const int idCol = 0;
			static const int fromIDCol = 1;
			static const int toIDCol = 2;
			static const int delayCol = 3;
			static const int weightCol = 4;
			static const int tmpWeightCol = 5;

			//=====================  METHODS  ======================
    };

}

#endif//CONNECTIONSMODEL_H

