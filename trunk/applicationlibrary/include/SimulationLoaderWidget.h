#ifndef SIMULATIONLOADERWIDGET_H
#define SIMULATIONLOADERWIDGET_H

//Qt includes
#include <QWidget>
#include <QHash>
#include <QLayout>
#include <QComboBox>
#include <QStackedWidget>

namespace spikestream {

	/*! Holds the different types of simulation widget and allows
		selection using a combo box.  */
	class SimulationLoaderWidget : public QWidget {
	  Q_OBJECT

		public:
			SimulationLoaderWidget(QWidget* parent=0);
			~SimulationLoaderWidget();

		private slots:
			void showSimulationWidget(int layerID);

		private:
			//=========================  VARIABLES  =============================
			/*! Combo box to select plugin */
			QComboBox* pluginsCombo;

			/*! Stores the position in the stacked widget where the analysis plugins are stored. */
			QHash<QString, int> pluginWidgetMap;

			/*! Vertical box organizing layout */
			QVBoxLayout *mainVerticalBox;

			/*! Widget with layers holding the different analysis plugins */
			QStackedWidget* stackedWidget;
	};

}

#endif//SIMULATIONLOADERWIDGET_H
