#ifndef NEURONGROUPMODEL_H
#define NEURONGROUPMODEL_H

//SpikeStream includes
#include "NeuronGroupInfo.h"
using namespace spikestream;

//Qt includes
#include <QAbstractTableModel>
#include <QStringList>

namespace spikestream {

    class NeuronGroupModel : public QAbstractTableModel {
	Q_OBJECT

	public:
	    NeuronGroupModel();
	    ~NeuronGroupModel();
	    int columnCount(const QModelIndex& parent = QModelIndex()) const;
	    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
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

	    static const int numCols = 6;
	    static const int visCol = 0;
	    static const int zoomCol = 1;
	    static const int idCol = 2;
	    static const int nameCol = 3;
	    static const int descCol = 4;
	    static const int neurTypeCol = 5;
    };

}

#endif//NEURONGROUPMODEL_H

