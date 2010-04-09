#ifndef LIVELINESSGRAPHDIALOG_H
#define LIVELINESSGRAPHDIALOG_H

//SpikeStream includes
#include "AbstractGraphDialog.h"
using namespace spikestream;

//Qt includes
#include <QComboBox>

namespace spikestream {

	class LivelinessGraphDialog : public AbstractGraphDialog {
		Q_OBJECT

		public:
			LivelinessGraphDialog(QWidget* parent, const AnalysisInfo& info);
			~LivelinessGraphDialog();

		private:
			void addTimeSteps(QComboBox* combo);
	};

}

#endif//LIVELINESSGRAPHDIALOG_H
