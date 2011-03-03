#ifndef ABSTRACTDEVICEWIDGET_H
#define ABSTRACTDEVICEWIDGET_H

//SpikeStream includes
#include "AbstractDeviceManager.h"

//Qt includes
#include <QWidget>

namespace spikestream {

	class AbstractDeviceWidget : public QWidget {

		public:
			AbstractDeviceWidget(QWidget* parent= 0);
			virtual ~AbstractDeviceWidget();
			virtual AbstractDeviceManager* getDeviceManager() = 0;

	};

}
#endif//ABSTRACTDEVICEWIDGET_H

