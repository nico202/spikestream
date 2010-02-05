#ifndef ABSTRACTANALYSISWIDGET_H
#define ABSTRACTANALYSISWIDGET_H

//SpikeStream includes
#include "AnalysisRunner.h"
using namespace spikestream;

//Qt includes
#include <QComboBox>
#include <QToolBar>
#include <QWidget>

namespace spikestream {

	/*! Abstract class that handles generic functions likely to be
		common to all analysis widgets. */
	class AbstractAnalysisWidget : public QWidget {
		Q_OBJECT

		public:
			AbstractAnalysisWidget(QWidget* parent);
			virtual ~AbstractAnalysisWidget();

		protected slots:
			virtual void archiveChanged();
			void checkToolBarEnabled();
			void fixTimeStepSelection(int);
			virtual void exportAnalysis() = 0;
			virtual void loadAnalysis();
			void loadArchiveTimeStepsIntoCombos();
			virtual void networkChanged();
			virtual void newAnalysis() = 0;
			virtual void selectParameters();
			virtual void startAnalysis() = 0;
			virtual void stopAnalysis();
			virtual void threadFinished();
			virtual void updateResults() = 0;


		protected:
			//=======================  VARIABLES  ========================
			/*! Toolbar with controls for the widget */
			QToolBar* toolBar;

			/*! Allows user to select first time step to be analyzed */
			QComboBox* fromTimeStepCombo;

			/*! Allows user to select last time step to be analyzed */
			QComboBox* toTimeStepCombo;

			/*! Class responsible for running the analysis */
			AnalysisRunner* analysisRunner;

			/*! Information about the analysis */
			AnalysisInfo analysisInfo;

			/*! The task that is currently being undertaken */
			int currentTask;

			/*! Task not defined */
			static const int UNDEFINED_TASK = 0;

			/*! Task of Liveliness analysis */
			static const int ANALYSIS_TASK = 1;

			//==========================  METHODS  ========================
			QToolBar* getDefaultToolBar();
			int getFirstTimeStep();
			int getLastTimeStep();
			QStringList getTimeStepList(unsigned int min, unsigned int max);
	};

}

#endif//ABSTRACTANALYSISWIDGET_H

