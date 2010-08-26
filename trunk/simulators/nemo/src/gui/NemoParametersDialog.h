#ifndef NEMOPARAMETERSDIALOG_H
#define NEMOPARAMETERSDIALOG_H

//SpikeStream includes


//Qt includes
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>

//Other includes
#include "nemo.h"


namespace spikestream {

	/*! Displays the current  parameters for the Nemo CUDA simulator. */
	class NemoParametersDialog : public QDialog {
		Q_OBJECT

		public:
			NemoParametersDialog(nemo_configuration_t nemoConfig, unsigned stdpFunctionID, QWidget* parent=0);
			~NemoParametersDialog();
			nemo_configuration_t getNemoConfig(){ return currentNemoConfig; }
			unsigned getSTDPFunctionID() { return stdpFunctionID; }

		private slots:
			void backendChanged(int index);
			void defaultButtonClicked();
			void okButtonClicked();
			void stdpParameterButtonClicked();


		private:
			//=====================  VARIABLES  ======================
			/*! Current configuration */
			nemo_configuration_t currentNemoConfig;

			/*! Default optimal configuration created by NeMo */
			nemo_configuration_t defaultNemoConfig;

			/*! The current stdp function ID */
			unsigned stdpFunctionID;

			/*! Combo box allowing selection of backend */
			QComboBox* backendCombo;

			/*! Combo containing current CUDA devices */
			QComboBox* cudaDeviceCombo;

			/*! Label for CUDA devices, which needs to be shown and hidden. */
			QLabel* cudaDeviceLabel;

			/*! Text field for editing the number of CPU threads. */
			QLineEdit* threadsLineEdit;

			/*! Label for number of threads, which needs to be shown and hidden. */
			QLabel* threadsLabel;

			/*! Combo box to select STDP function */
			QComboBox* stdpCombo;


			//=========================  METHODS  ===========================
			void addButtons(QVBoxLayout* mainVLayout);
			void checkNemoOutput(nemo_status_t result, const QString& errorMessage);
			void getCudaDevices(QComboBox* combo);
			void getStdpFunctions(QComboBox* combo);
			void loadParameters(nemo_configuration_t config);
			void storeParameterValues();
	};

}

#endif//NEMOPARAMETERSDIALOG_H

