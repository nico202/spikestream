#ifndef LIVELINESSWIDGET_H
#define LIVELINESSWIDGET_H

//SpikeStream includes
#include "AbstractAnalysisWidget.h"
#include "FullResultsModel.h"
#include "FullResultsTableView.h"
#include "ProgressWidget.h"
#include "LivelinessAnalysisDao.h"
using namespace spikestream;

//Qt includes
#include <QTableView>

namespace spikestream {

	class LivelinessWidget : public AbstractAnalysisWidget {
	    Q_OBJECT

		public:
			LivelinessWidget(QWidget* parent = 0);
			~LivelinessWidget();

		private:
			//=====================  VARIABLES  =======================
			/*! Wraps analysis database with methods specialized for a state based phi analysis */
			LivelinessAnalysisDao* livelinessDao;

			/*! Shows a model derived from the liveliness data table */
			QTableView* analysisDataTableView;

			/*! Displays progress bars for each of the threads analyzing time steps */
			ProgressWidget* progressWidget;

			/*! Model for displaying table of results */
			FullResultsModel* fullResultsModel;


			//=====================  METHODS  =======================
   };

}

#endif // LIVELINESSWIDGET_H
