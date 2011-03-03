#ifndef DEVICELOADERWIDGET_H
#define DEVICELOADERWIDGET_H

//SpikeStream includes
#include "AbstractDeviceWidget.h"

//Qt includes
#include <QComboBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace spikestream {

	/*! Loads experiments and allows user to switch between them using a combo box. */
	class DeviceLoaderWidget : public QWidget {
		Q_OBJECT

		  public:
			  DeviceLoaderWidget(QString pluginPath, QWidget* parent=0);
			  ~DeviceLoaderWidget();
			  QList<AbstractDeviceWidget*> getAbstractDeviceWidgets() {return abstractDeviceList; }

		  private slots:
			  void showDeviceWidget(int layerID);

		  private:
			  //=========================  VARIABLES  =============================
			  /*! Combo box to select plugin */
			  QComboBox* pluginsCombo;

			  /*! Stores the position in the stacked widget where the device plugins are stored. */
			  QHash<QString, int> pluginWidgetMap;

			  /*! List of pointers to the device plugins */
			  QList<AbstractDeviceWidget*> abstractDeviceList;

			  /*! Vertical box organizing layout */
			  QVBoxLayout *mainVerticalBox;

			  /*! Widget with layers holding the different experiment plugins */
			  QStackedWidget* stackedWidget;
	};

}

#endif//EXPERIMENTLOADERWIDGET_H
