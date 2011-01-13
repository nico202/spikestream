#ifndef SPIKERASTERDIALOG_H
#define SPIKERASTERDIALOG_H

//SpikeStream includes
#include "NeuronGroup.h"
#include "SpikeRasterWidget.h"

//Qt includes
#include <QDialog>

namespace spikestream {

	class SpikeRasterDialog : public QDialog {
		Q_OBJECT

		public:
			SpikeRasterDialog(QList<NeuronGroup*> neuronGroupList, QWidget* parent = 0);
			~SpikeRasterDialog();
			void addData(const QList<unsigned>& firingNeuronIDs, unsigned timeStep);

		private:
			//====================  VARIABLES  ======================
			SpikeRasterWidget* spikeRasterWidget;

	};

}

#endif//SPIKERASTERDIALOG_H
