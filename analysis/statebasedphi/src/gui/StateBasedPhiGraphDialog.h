#ifndef STATEBASEDPHIGRAPHDIALOG_H
#define STATEBASEDPHIGRAPHDIALOG_H

//SpikeStream includes
#include "AbstractGraphDialog.h"
using namespace spikestream;

//Qt includes
#include <QComboBox>

namespace spikestream {

	class StateBasedPhiGraphDialog : public AbstractGraphDialog {
		Q_OBJECT

		public:
			StateBasedPhiGraphDialog(QWidget* parent, const AnalysisInfo& info);
			~StateBasedPhiGraphDialog();

		private:
			void addTimeSteps(QComboBox* combo);
	};

}

#endif//STATEBASEDPHIGRAPHDIALOG
