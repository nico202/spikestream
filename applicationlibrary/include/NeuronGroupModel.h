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
			QList<unsigned int> getSelectedNeuronGroupIDs();
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

		private slots:
			void networkChanged();
			void networkDisplayChanged();

		private:
			//====================  VARIABLES  ====================
			/*! List containing the information about the neuron groups in the current network
				This list is automatically refreshed when the network changes. */
			QList<NeuronGroupInfo> neurGrpInfoList;

			/*! Map of indexes of selected rows in neuron group info list */
			QHash<unsigned int, bool> selectionMap;

			static const int numCols = 7;
			static const int visCol = 0;
			static const int zoomCol = 1;
			static const int idCol = 2;
			static const int nameCol = 3;
			static const int descCol = 4;
			static const int neurTypeCol = 5;
			static const int selectCol = 6;
		};

}

#endif//NEURONGROUPMODEL_H

