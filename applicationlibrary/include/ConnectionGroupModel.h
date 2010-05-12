#ifndef CONNECTIONGROUPMODEL_H
#define CONNECTIONGROUPMODEL_H

//SpikeStream includes
#include "ConnectionGroupInfo.h"
using namespace spikestream;

//Qt includes
#include <QAbstractTableModel>
#include <QStringList>

namespace spikestream {

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
	    int rowCount(const QModelIndex& parent = QModelIndex()) const;
	    bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

	private slots:
	    void networkChanged();

	private:
	    //====================  VARIABLES  ====================
	    /*! List containing the information about the neuron groups in the current network
		This list is automatically refreshed when the network changes. */
	    QList<ConnectionGroupInfo> conGrpInfoList;

		/*! Map of indexes of selected rows in connection group info list */
		QHash<unsigned int, bool> selectionMap;

		static const int numCols = 7;
	    static const int visCol = 0;
	    static const int idCol = 1;
	    static const int descCol = 2;
	    static const int fromNeurIDCol = 3;
	    static const int toNeurIDCol = 4;
	    static const int synapseTypeCol = 5;
		static const int selectCol = 6;
    };

}

#endif//CONNECTIONGROUPMODEL_H

