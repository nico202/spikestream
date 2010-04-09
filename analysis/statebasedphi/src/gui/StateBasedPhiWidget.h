#ifndef STATEBASEDPHIWIDGET_H
#define STATEBASEDPHIWIDGET_H

//SpikeStream includes
#include "AbstractAnalysisWidget.h"
#include "AnalysisInfo.h"
#include "AnalysisRunner.h"
#include "FullResultsModel.h"
#include "FullResultsTableView.h"
#include "GlobalVariables.h"
#include "ProgressWidget.h"
#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

//Qt includes
#include <QWidget>
#include <QToolBar>
#include <QTableView>
#include <QComboBox>
#include <QStringList>

namespace spikestream {

	class StateBasedPhiWidget : public AbstractAnalysisWidget {
		Q_OBJECT

		public:
			StateBasedPhiWidget(QWidget* parent = 0);
			~StateBasedPhiWidget();
			void hideAnalysisResults();

		protected:
			void initializeAnalysisInfo();

		private slots:
			void exportAnalysis();
			QString getAnalysisName() { return STATE_BASED_PHI_ANALYSIS; }
			void newAnalysis();
			void plotGraphs();
			void startAnalysis();
			void updateResults();

		private:
			//=====================  VARIABLES  =======================
			/*! Wraps analysis database with methods specialized for a state based phi analysis */
			StateBasedPhiAnalysisDao* stateDao;

			/*! Shows a model derived from the state based phi data table */
			QTableView* analysisDataTableView;

			/*! Displays progress bars for each of the threads analyzing time steps */
			ProgressWidget* progressWidget;

			/*! Model for displaying table of results */
			FullResultsModel* fullResultsModel;


			//=====================  METHODS  =======================
			bool timeStepsAlreadyAnalyzed(int firstTimeStep, int lastTimeStep);
	};

}

#endif // STATEBASEDPHIWIDGET_H
