#ifndef POP1EXPERIMENTWIDGET_H
#define POP1EXPERIMENTWIDGET_H

//SpikeStream includes
#include "AbstractExperimentWidget.h"
#include "Pop1ExperimentManager.h"
#include "NemoWrapper.h"

//Qt includes
#include <QToolBar>
#include <QWidget>

namespace spikestream {

	/*! Graphical interface for carrying out a pop1 experiment */
	class Pop1ExperimentWidget : public AbstractExperimentWidget {
		Q_OBJECT

		public:
			Pop1ExperimentWidget(QWidget* parent = 0);
			~Pop1ExperimentWidget();
			void setWrapper(void *wrapper);

		private slots:
			void managerFinished();
			void startExperiment();
			void stopExperiment();

		private:
			//=====================  VARIABLES  ====================
			/*! Wrapper of NeMo that is used in the experiments. */
			NemoWrapper* nemoWrapper;

			/*! Manager that runs the experiments */
			Pop1ExperimentManager* pop1ExperimentManager;


			//=====================  METHODS  ======================
			void checkNetwork();

	};

}

#endif//POP1EXPERIMENTWIDGET_H

