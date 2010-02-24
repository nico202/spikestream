#ifndef LIVELINESSFULLRESULTSMODEL_H
#define LIVELINESSFULLRESULTSMODEL_H

//SpikeStream includes
#include "AnalysisInfo.h"
#include "Cluster.h"
#include "LivelinessDao.h"
using namespace spikestream;

//Qt includes
#include <QAbstractTableModel>
#include <QList>
#include <QHash>

namespace spikestream {

	class LivelinessFullResultsModel : public QAbstractTableModel {
		Q_OBJECT

		public:
			LivelinessFullResultsModel(const AnalysisInfo* analysisInfo, LivelinessDao* livelinessDao);
			~LivelinessFullResultsModel();
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
			double getMaxHeatColorValue();
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			void hideClusters();
			void reload();
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			bool setData(const QModelIndex& index, const QVariant&, int);
			void setMaxHeatColorValue(double maxHeatColorValue);

		private:
			//====================  VARIABLES  ====================
			/*! List containing the information about the clusters that are currently loaded. */
			QList<Cluster> clusterList;

			/*! The index in the clusterList of which cluster is visible.
				Set to -1 if no clusters are visible */
			int clusterDisplayIndex;

			/*! Details about the analysis.
			Points to the analysis info held in the containing class. */
			const AnalysisInfo* analysisInfo;

			/*! Pointer to the database dao */
			LivelinessDao* livelinessDao;

			/*! Max value of the colour map */
			double maxHeatColorValue;

			static const int numCols = 5;
			static const int idCol = 0;
			static const int timeStepCol = 1;
			static const int livelinessCol = 2;
			static const int neurCol = 3;
			static const int viewCol = 4;


			//=====================  METHODS  ======================
			void clearClusters();
			void setVisibleCluster(int index);
			void setVisibility(Cluster& cluster, bool visible);
	};

}

#endif//LIVELINESSFULLRESULTSMODEL_H

