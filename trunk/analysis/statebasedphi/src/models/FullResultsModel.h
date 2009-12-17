#ifndef FULLRESULTSMODEL_H
#define FULLRESULTSMODEL_H

//SpikeStream includes
#include "AnalysisInfo.h"
#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

//Qt includes
#include <QAbstractTableModel>
#include <QList>
#include <QHash>

namespace spikestream {

    class FullResultsModel : public QAbstractTableModel {
	Q_OBJECT

	public:
	    FullResultsModel(const AnalysisInfo* analysisInfo, StateBasedPhiAnalysisDao* stateDao);
	    ~FullResultsModel();
	    int columnCount(const QModelIndex& parent = QModelIndex()) const;
	    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	    void reload();
	    int rowCount(const QModelIndex& parent = QModelIndex()) const;
	    bool setData(const QModelIndex& index, const QVariant&, int);

	private:
	    //====================  VARIABLES  ====================
	    /*! List containing the information about the complexes that are currently loaded. */
	    QList<Complex> complexList;

	    /*! The index in the complexList of which complex is visible.
		Set to -1 if no complexes are visible */
	    int complexDisplayIndex;

	    /*! Details about the analysis.
		Points to the analysis info held in the containing class. */
	    const AnalysisInfo* analysisInfo;

	    /*! Pointer to the database dao */
	    StateBasedPhiAnalysisDao* stateDao;

	    static const int numCols = 5;
	    static const int idCol = 0;
	    static const int timeStepCol = 1;
	    static const int phiCol = 2;
	    static const int neurCol = 3;
	    static const int viewCol = 4;


	    //=====================  METHODS  ======================
	    void clearComplexes();
	    void setVisibleComplex(int index);
	    void setVisibility(Complex& complex, bool visible);
    };

}

#endif//FULLRESULTSMODEL_H

