#ifndef STATEBASEDPHIWIDGET_H
#define STATEBASEDPHIWIDGET_H

//SpikeStream includes
#include "AnalysisInfo.h"
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
		void newAnalysis();
		void selectParameters();
		void startAnalysis();
		void stopAnalysis();

	    private:
		//=====================  VARIABLES  =======================
		/*! Wraps analysis database with methods specialized for a state based phi analysis */
		StateBasedPhiAnalysisDao* stateDao;

		/*! Shows a model derived from the state based phi data table */
		QTableView* analysisDataTableView;

		/*! Toolbar with controls for the widget */
		QToolBar* toolBar;

		QComboBox* fromTimeStepCombo;
		QComboBox* toTimeStepCombo;


		/*! Information about the analysis */
		AnalysisInfo analysisInfo;

		//=====================  METHODS  =======================
		bool analysisLoaded();
		QStringList getTimeStepList(unsigned int min, unsigned int max);
		QToolBar* getToolBar();
    };

}

#endif // STATEBASEDPHIWIDGET_H
