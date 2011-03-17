#ifndef IZHIACCURACYEXPTWIDGET_H
#define IZHIACCURACYEXPTWIDGET_H

//SpikeStream includes
#include "AbstractExperimentWidget.h"
#include "IzhiAccuracyManager.h"
#include "NemoWrapper.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>
#include <QTextEdit>

namespace spikestream {

	/*! Graphical interface for carrying out a pop1 experiment */
	class IzhiAccuracyExptWidget : public AbstractExperimentWidget {
		Q_OBJECT

		public:
			IzhiAccuracyExptWidget(QWidget* parent = 0);
			~IzhiAccuracyExptWidget();
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
			IzhiAccuracyManager* izhiAccuracyManager;

			/*! Widget holding messages from the manager */
			QTextEdit* statusTextEdit;


			//=====================  METHODS  ======================
			void checkNetwork();

	};

}

#endif//IZHIACCURACYEXPTWIDGET_H

