#ifndef POP1EXPERIMENTWIDGET_H
#define POP1EXPERIMENTWIDGET_H

//SpikeStream includes
#include "AbstractExperimentWidget.h"
#include "Pop1ExperimentManager.h"
#include "NemoWrapper.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>
#include <QPushButton>
#include <QTextEdit>
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
			void updateStatus(QString statusMsg);

		protected:
			void buildParameters();

		private:
			//=====================  VARIABLES  ====================
			/*! Wrapper of NeMo that is used in the experiments. */
			NemoWrapper* nemoWrapper;

			/*! Manager that runs the experiments */
			Pop1ExperimentManager* pop1ExperimentManager;


			/*! Widget holding messages from the manager */
			QTextEdit* statusTextEdit;
			/*! Start neuron id in the experiment.
				Neuron ids are expected to increase
				by 1 along Y axis and by 10 along X axis. */
			NeuronGroup* neuronGroup;

			//=====================  METHODS  ======================
			void checkNetwork();

	};

}

#endif//POP1EXPERIMENTWIDGET_H

