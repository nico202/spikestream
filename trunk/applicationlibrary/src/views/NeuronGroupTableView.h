#ifndef NEURONGROUPTABLEVIEW_H
#define NEURONGROUPTABLEVIEW_H

//Qt includes
#include <QTableView>
#include <QAbstractTableModel>

namespace spikestream {

    class NeuronGroupTableView : public QTableView {
	Q_OBJECT

	public:
	    NeuronGroupTableView(QAbstractTableModel* model);
	    ~NeuronGroupTableView();
	    void resizeHeaders();

	private slots:
	    void tableClicked(QModelIndex index);

    };

}

#endif//NEURONGROUPTABLEVIEW_H
