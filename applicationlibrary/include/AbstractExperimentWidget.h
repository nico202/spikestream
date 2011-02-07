#ifndef ABSTRACTEXPERIMENT_H
#define ABSTRACTEXPERIMENT_H

//Qt includes
#include <QToolBar>
#include <QWidget>

namespace spikestream {

	class AbstractExperimentWidget : public QWidget {
		Q_OBJECT

		public:
			AbstractExperimentWidget(QWidget* parent= 0);
			virtual ~AbstractExperimentWidget();
			virtual void setWrapper(void* wrapper) = 0;


		signals:
			void experimentStarted();
			void experimentEnded();


		protected slots:
			virtual void startExperiment() = 0;
			virtual void stopExperiment() = 0;


		protected:
			//=====================  VARIABLES  =======================
			/*! Action to start experiment */
			QAction* playAction;

			/*! Action to stop experiment. */
			QAction* stopAction;

			//======================  METHODS  ========================
			QToolBar* getToolBar();

	};

}
#endif//ABSTRACTEXPERIMENT_H

