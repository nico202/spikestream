#ifndef TRANSFERENTROPYWIDGET_H
#define TRANSFERENTROPYWIDGET_H

//SpikeStream includes
#include "AbstractAnalysisWidget.h"
#include "GlobalVariables.h"
#include "SpikeStreamTypes.h"
using namespace spikestream;

//Qt includes
#include <QTextEdit>

#define TRANSFER_ENTROPY_ANALYSIS "transferentropyanalysis"

namespace spikestream {

	class TransferEntropyWidget : public AbstractAnalysisWidget {
		Q_OBJECT

		public:
			TransferEntropyWidget(QWidget* parent = 0);
			~TransferEntropyWidget();
			QString getAnalysisName() { return TRANSFER_ENTROPY_ANALYSIS; }
			void exportAnalysis();
			void hideAnalysisResults();
			void newAnalysis();
			void plotGraphs();
			void startAnalysis();
			void updateResults();

		protected:
			void initializeAnalysisInfo();

		private slots:


		private:
			//=====================  VARIABLES  =======================
			/*! Displays information about analysis */
			QTextEdit* statusTextEdit;

			/*! From neuron */
			neurid_t fromNeuronID;

			/*! To neuron id */
			neurid_t toNeuronID;

			/*! Time window within which the transfer entropy is calculated */
			unsigned timeWindow;

			/*! k parameter of analysis */
			unsigned k_param;

			/*! l parameter of analysis */
			unsigned l_param;

			//=====================  METHODS  =======================
			void storeParameters();

   };

}

#endif // TRANSFERENTROPYWIDGET_H
