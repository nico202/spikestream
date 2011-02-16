#ifndef MEMBRANEPOTENTIALGRAPHDIALOG_H
#define MEMBRANEPOTENTIALGRAPHDIALOG_H

//SpikeStream includes
#include "MembranePotentialGraphWidget.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QDialog>

namespace spikestream {

	class MembranePotentialGraphDialog : public QDialog {
		Q_OBJECT

		public:
			MembranePotentialGraphDialog(neurid_t neuronID, QWidget* parent = 0);
			~MembranePotentialGraphDialog();
			void addData(float membranePotential, timestep_t timeStep);


		private:
			//====================  VARIABLES  ======================
			MembranePotentialGraphWidget* memPotWidget;

	};

}

#endif//MEMBRANEPOTENTIALGRAPHDIALOG_H
