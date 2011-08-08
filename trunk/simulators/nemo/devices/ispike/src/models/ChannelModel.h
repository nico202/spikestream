#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

//SpikeStream includes
#include "ISpikeManager.h"

//Qt includes
#include <QAbstractTableModel>
#include <QStringList>

namespace spikestream {

	/*! Used for the display of information about a neuron group.
		Based on the NeuronGroup table in the SpikeStream database. */
	class ChannelModel : public QAbstractTableModel {
		Q_OBJECT

		public:
			ChannelModel(ISpikeManager* iSpikeManager);
			~ChannelModel();
			void clearSelection();
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
			void deleteChannel(int row);
			map<string, Property> getProperties(int row);
			void setProperties(int row, map<string, Property> propertyMap);
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			bool isEmpty();
			void reload();
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

			friend class ChannelTableView;



		private:
			//====================  VARIABLES  ====================
			/*! List containing the names of the channels */
			QList<QString> channelNameList;

			/*! List containing the names of the associated neuron groups */
			QList<QString> neuronGroupNameList;

			/*! Manager holding information about the channels */
			ISpikeManager* iSpikeManager;

			static const int NUM_COLS = 4;
			static const int CHANNEL_NAME_COL = 0;
			static const int NEURON_GROUP_NAME_COL = 1;
			static const int PARAM_COL = 2;
			static const int DELETE_COL = 3;


			//====================  METHODS  =====================
			QString getNeuronGroupName(NeuronGroup* neuronGroup);
			void loadChannels();

		};

}

#endif//CHANNELMODEL_H

