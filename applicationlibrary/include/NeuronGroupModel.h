#ifndef NEURONGROUPMODEL_H
#define NEURONGROUPMODEL_H

//SpikeStream includes
#include "NeuronGroupInfo.h"
using namespace spikestream;

//Qt includes
#include <QAbstractTableModel>
#include <QStringList>

namespace spikestream {

	/*! Used for the display of information about a neuron group.
		Based on the NeuronGroup table in the SpikeStream database. */
    class NeuronGroupModel : public QAbstractTableModel {
		Q_OBJECT

		public:
			NeuronGroupModel();
			~NeuronGroupModel();
			void clearSelection();
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
			NeuronGroupInfo getInfo(const QModelIndex& index) const;
			QHash<QString, double> getParameters(int row);
			QList<unsigned int> getSelectedNeuronGroupIDs();
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			void reload();
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			void selectAllOrNone();
			void showAllOrNone();
			bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

			friend class NeuronGroupTableView;

		private slots:
			void loadNeuronGroups();
			void networkDisplayChanged();

		private:
			//====================  VARIABLES  ====================
			/*! List containing the information about the neuron groups in the current network
				This list is automatically refreshed when the network changes. */
			QList<NeuronGroupInfo> neurGrpInfoList;

			/*! List holding the sizes of each neuron group */
			QList<unsigned> neurGrpSizeList;

			/*! Map of indexes of selected rows in neuron group info list */
			QHash<unsigned int, bool> selectionMap;

			static const int NUM_COLS = 9;
			static const int VIS_COL = 0;
			static const int ZOOM_COL = 1;
			static const int ID_COL = 2;
			static const int NAME_COL = 3;
			static const int DESC_COL = 4;
			static const int SIZE_COL = 5;
			static const int NEUR_TYPE_COL = 6;
			static const int SELECT_COL = 7;
			static const int PARAM_COL = 8;
		};

}

#endif//NEURONGROUPMODEL_H

