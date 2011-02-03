#ifndef EXPERIMENTLOADERWIDGET_H
#define EXPERIMENTLOADERWIDGET_H

//SpikeStream includes
#include "AbstractExperimentWidget.h"

//Qt includes
#include <QComboBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace spikestream {

	/*! Loads experiments and allows user to switch between them using a combo box. */
	class ExperimentLoaderWidget : public QWidget {
		Q_OBJECT

		  public:
			  ExperimentLoaderWidget(QString pluginPath, QWidget* parent=0);
			  ~ExperimentLoaderWidget();
			  QList<AbstractExperimentWidget*> getAbstractExperimentWidgets() {return abstractExperimentList; }

		  private slots:
			  void showExperimentWidget(int layerID);

		  private:
			  //=========================  VARIABLES  =============================
			  /*! Combo box to select plugin */
			  QComboBox* pluginsCombo;

			  /*! Stores the position in the stacked widget where the experiment plugins are stored. */
			  QHash<QString, int> pluginWidgetMap;

			  /*! List of pointers to the experiment plugins */
			  QList<AbstractExperimentWidget*> abstractExperimentList;

			  /*! Vertical box organizing layout */
			  QVBoxLayout *mainVerticalBox;

			  /*! Widget with layers holding the different experiment plugins */
			  QStackedWidget* stackedWidget;
	};

}

#endif//EXPERIMENTLOADERWIDGET_H
