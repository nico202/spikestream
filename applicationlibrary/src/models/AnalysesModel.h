#ifndef ANALYSESMODEL_H
#define ANALYSESMODEL_H

//SpikeStream includes
#include "AnalysisInfo.h"
using namespace spikestream;

//Qt includes
#include <QAbstractTableModel>
#include <QList>
#include <QHash>

namespace spikestream {

    class AnalysesModel : public QAbstractTableModel {
	Q_OBJECT

	public:
	    AnalysesModel();
	    ~AnalysesModel();
	    int columnCount(const QModelIndex& parent = QModelIndex()) const;
	    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	    QList<AnalysisInfo> getSelectedAnalyses();
	    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	    void reload();
	    int rowCount(const QModelIndex& parent = QModelIndex()) const;
	    bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);

	private:
	    //====================  VARIABLES  ====================
	    /*! List containing the information about the neuron groups in the current network
		This list is automatically refreshed when the network changes. */
	    QList<AnalysisInfo> analysisInfoList;

	    /*! Map of the analyses that are currently selected.
		The key is the index in the analysis info list; the value is a bool. */
	    QHash<int, bool> selectionMap;

	    static const int numCols = 8;
	    static const int selectCol = 0;
	    static const int idCol = 1;
	    static const int netIDCol = 2;
	    static const int archIDCol = 3;
	    static const int timeCol = 4;
	    static const int descCol = 5;
	    static const int paramCol = 6;
	    static const int typeCol = 7;
    };

}

#endif//ANALYSESMODEL_H

