#ifndef LIVELINESSWIDGET_H
#define LIVELINESSWIDGET_H

//SpikeStream includes
#include "AbstractAnalysisWidget.h"
#include "LivelinessFullResultsModel.h"
#include "LivelinessFullResultsTableView.h"
#include "ProgressWidget.h"
#include "LivelinessDao.h"
using namespace spikestream;

//Qt includes
#include <QTableView>

namespace spikestream {

	class LivelinessWidget : public AbstractAnalysisWidget {
		Q_OBJECT

		public:
			LivelinessWidget(QWidget* parent = 0);
			~LivelinessWidget();
			void exportAnalysis();
			void newAnalysis();
			void startAnalysis();
			void updateResults();

		private:
			//=====================  VARIABLES  =======================
			/*! Wraps analysis database with methods specialized for a state based phi analysis */
			LivelinessDao* livelinessDao;

			/*! Shows a model derived from the liveliness data table */
			QTableView* analysisDataTableView;

			/*! Displays progress bars for each of the threads analyzing time steps */
			ProgressWidget* progressWidget;

			/*! Model for displaying table of results */
			LivelinessFullResultsModel* fullResultsModel;


			//=====================  METHODS  =======================
			void initializeAnalysisInfo();
			bool timeStepsAlreadyAnalyzed(int firstTimeStep, int lastTimeStep);
   };

}

#endif // LIVELINESSWIDGET_H
