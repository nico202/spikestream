#ifndef NEURONGROUPSELECTIONMODEL_H
#define NEURONGROUPSELECTIONMODEL_H

//SpikeStream includes
#include "NeuronGroup.h"
using namespace spikestream;

//Qt includes
#include <QAbstractTableModel>
#include <QStringList>

namespace spikestream {

	/*! Used for the display of selectable list of neuron groups. */
	class NeuronGroupSelectionModel : public QAbstractTableModel {
		Q_OBJECT

		public:
			NeuronGroupSelectionModel(QList<NeuronGroup*> neuronGroupList);
			~NeuronGroupSelectionModel();
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
			QList<NeuronGroup*> getSelectedNeuronGroups();
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			void selectAllOrNone();
			bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

			friend class NeuronGroupSelectionView;


		private:
			//====================  VARIABLES  ====================
			/*! List containing the neuron groups in the current network
				This list is automatically refreshed when the network changes. */
			QList<NeuronGroup*> neurGrpList;

			/*! Map of indexes of selected rows in neuron group list */
			QHash<unsigned, bool> selectionMap;

			static const int NUM_COLS = 3;
			static const int SELECT_COL = 0;
			static const int NAME_COL = 1;
			static const int ID_COL = 2;
		};

}

#endif//NEURONGROUPSELECTIONMODEL_H

