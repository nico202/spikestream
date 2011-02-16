#ifndef ABSTRACTEXPERIMENT_H
#define ABSTRACTEXPERIMENT_H

//SpikeStream includes
#include "ParameterInfo.h"

//Qt includes
#include <QHash>
#include <QPushButton>
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
			void parametersButtonClicked();
			virtual void startExperiment() = 0;
			virtual void stopExperiment() = 0;


		protected:
			//=====================  VARIABLES  =======================
			/*! Action to start experiment */
			QAction* playAction;

			/*! Action to stop experiment. */
			QAction* stopAction;

			/*! Button to set parameters */
			QPushButton* parametersButton;

			/*! Current parameters for the experiment */
			QHash<QString, double> parameterMap;

			/*! Default arameters for the experiment */
			QHash<QString, double> defaultParameterMap;

			/*! Meta information about the parameters fro the experiment. */
			QList<ParameterInfo> parameterInfoList;


			//======================  METHODS  ========================
			virtual void buildParameters() = 0;
			QToolBar* getToolBar();

		private:
			/*! Records that parameters have been initialized */
			bool parametersBuilt;

	};

}
#endif//ABSTRACTEXPERIMENT_H

