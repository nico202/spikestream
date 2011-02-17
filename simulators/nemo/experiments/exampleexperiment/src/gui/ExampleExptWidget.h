#ifndef TEMPORALCODINGEXPTWIDGET_H
#define TEMPORALCODINGEXPTWIDGET_H

//SpikeStream includes
#include "AbstractExperimentWidget.h"
#include "TemporalCodingExptManager.h"
#include "NemoWrapper.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>
#include <QTextEdit>

namespace spikestream {

	/*! Graphical interface for carrying out a pop1 experiment */
	class TemporalCodingExptWidget : public AbstractExperimentWidget {
		Q_OBJECT

		public:
			TemporalCodingExptWidget(QWidget* parent = 0);
			~TemporalCodingExptWidget();
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
			TemporalCodingExptManager* temporalCodingExptManager;

			/*! Widget holding messages from the manager */
			QTextEdit* statusTextEdit;


			//=====================  METHODS  ======================
			void checkNetwork();

	};

}

#endif//TEMPORALCODINGEXPTWIDGET_H

