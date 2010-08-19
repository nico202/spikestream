#ifndef CONNECTIONGROUPMODEL_H
#define CONNECTIONGROUPMODEL_H

//SpikeStream includes
#include "ConnectionGroupInfo.h"
using namespace spikestream;

//Qt includes
#include <QAbstractTableModel>
#include <QStringList>

namespace spikestream {

	/*! Model used for the display and manipulation of connection groups.
		Based on the ConnectionGroup table in the SpikeStreamNetwork database. */
    class ConnectionGroupModel : public QAbstractTableModel {
		Q_OBJECT

		public:
			ConnectionGroupModel();
			~ConnectionGroupModel();
			void clearSelection();
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
			QList<unsigned int> getSelectedConnectionGroupIDs();
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			QHash<QString, double> getParameters(int row);
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

			friend class ConnectionGroupTableView;

		private slots:
			void networkChanged();

		private:
			//====================  VARIABLES  ====================
			/*! List containing the information about the neuron groups in the current network
			This list is automatically refreshed when the network changes. */
			QList<ConnectionGroupInfo> conGrpInfoList;

			/*! List containing the sizes of each connection group */
			QList<unsigned> conGrpSizeList;

			/*! Map of indexes of selected rows in connection group info list */
			QHash<unsigned int, bool> selectionMap;

			static const int NUM_COLS = 9;
			static const int VIS_COL = 0;
			static const int ID_COL = 1;
			static const int DESC_COL = 2;
			static const int SIZE_COL = 3;
			static const int FROM_NEUR_ID_COL = 4;
			static const int T0_NEUR_ID_COL = 5;
			static const int SYNAPSE_TYPE_COL = 6;
			static const int SELECT_COL = 7;
			static const int PARAM_COL = 8;
    };

}

#endif//CONNECTIONGROUPMODEL_H

