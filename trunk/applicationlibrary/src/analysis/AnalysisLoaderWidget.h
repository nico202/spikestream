#ifndef ANALYSISLOADERWIDGET_CPP
#define ANALYSISLOADERWIDGET_CPP

//Qt includes
#include <QWidget>
#include <QHash>
#include <QLayout>
#include <QComboBox>
#include <QStackedWidget>

namespace spikestream {

	/*! Holds the different types of analysis widget and allows
		selection using a combo box.  */
	class AnalysisLoaderWidget : public QWidget {
	  Q_OBJECT

		public:
			AnalysisLoaderWidget(QWidget* parent=0);
			~AnalysisLoaderWidget();

		private slots:
			void showAnalysisWidget(int layerID);

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

#endif // ANALYSISLOADERWIDGET_CPP
