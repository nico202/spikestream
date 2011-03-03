#ifndef ISPIKEWIDGET_H
#define ISPIKEWIDGET_H

//SpikeStream includes
#include "AbstractDeviceWidget.h"
#include "ISpikeManager.h"

//Qt includes
#include <QWidget>

namespace spikestream {

	/*! Interface used to set up communication between iSpike library and
		NeMo. */
	class ISpikeWidget : public AbstractDeviceWidget {
		Q_OBJECT

		public:
			ISpikeWidget(QWidget* parent = 0);
			~ISpikeWidget();
			AbstractDeviceManager* getDeviceManager();

		private:
			/*! Manager that interfaces with iSpike library */
			ISpikeManager* iSpikeManager;


	};
}

#endif//ISPIKEWIDGET_H
