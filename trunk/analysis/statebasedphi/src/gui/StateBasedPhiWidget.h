#ifndef STATEBASEDPHIWIDGET_H
#define STATEBASEDPHIWIDGET_H

//SpikeStream includes
#include "AnalysisInfo.h"
#include "AnalysisRunner.h"
#include "StateBasedPhiAnalysisDao.h"
using namespace spikestream;

//Qt includes
#include <QWidget>
#include <QToolBar>
#include <QTableView>
#include <QComboBox>
#include <QStringList>

namespace spikestream {

    class StateBasedPhiWidget : public QWidget {
	    Q_OBJECT

	    public:
		StateBasedPhiWidget(QWidget* parent = 0);
		~StateBasedPhiWidget();

	    private slots:
		void checkToolBarEnabled();
		void fixTimeStepSelection(int);
		void loadAnalysis();
		void loadArchiveTimeStepsIntoCombos();
		void newAnalysis();
		void selectParameters();
		void startAnalysis();
		void stopAnalysis();
		void threadFinished();
		void updateProgress(const QList< QList<unsigned int> >& progressList);
		void updateResults();

	    private:
		//=====================  VARIABLES  =======================
		/*! Wraps analysis database with methods specialized for a state based phi analysis */
		StateBasedPhiAnalysisDao* stateDao;

		/*! Shows a model derived from the state based phi data table */
		QTableView* analysisDataTableView;

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

		/*! Task of State based phi analysis */
		static const int ANALYSIS_TASK = 1;

		//=====================  METHODS  =======================
		bool analysisLoaded();
		QStringList getTimeStepList(unsigned int min, unsigned int max);
		QToolBar* getToolBar();
		void initializeParameters();
    };

}

#endif // STATEBASEDPHIWIDGET_H
