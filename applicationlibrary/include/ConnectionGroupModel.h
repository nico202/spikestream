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
	    int columnCount(const QModelIndex& parent = QModelIndex()) const;
	    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
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

	    static const int numCols = 6;
	    static const int visCol = 0;
	    static const int idCol = 1;
	    static const int descCol = 2;
	    static const int fromNeurIDCol = 3;
	    static const int toNeurIDCol = 4;
	    static const int synapseTypeCol = 5;
    };

}

#endif//CONNECTIONGROUPMODEL_H

