#ifndef TRUTHTABLEMODEL_H
#define TRUTHTABLEMODEL_H

//Qt includes
#include <QAbstractTableModel>
#include <QList>

namespace spikestream {

    class TruthTableModel : public QAbstractTableModel {
	Q_OBJECT

	public:
	    TruthTableModel();
	    ~TruthTableModel();
	    int columnCount(const QModelIndex& parent = QModelIndex()) const;
	    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	    int rowCount(const QModelIndex& parent = QModelIndex()) const;
	    void setNeuronID(unsigned int neuronID);

	private slots:


	private:
	    //====================  VARIABLES  ====================
	    /*! 2D List containing the data. Outer list corresponds to the rows
		inner list corresponds to the firing state of the pattern at each point in the row. */
	    QList< QList<unsigned int> > dataList;

	    /*! List containing the header data. Each position holds the appropriate neuron id for the column */
	    QList<unsigned int> headerList;

	    //=====================  METHODS  ======================
	    void clearModelData();
    };

}

#endif//TRUTHTABLEMODEL_H


