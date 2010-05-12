#ifndef RANDOM1WIDGET_H
#define RANDOM1WIDGET_H

//SpikeStream includes
#include "Random1BuilderThread.h"

//Qt includes
#include <QComboBox>
#include <QLineEdit>
#include <QProgressDialog>
#include <QWidget>

namespace spikestream {

	/*! Gathers information for creating a random connection pattern between two neuron groups. */
	class Random1Widget : public QWidget {
		Q_OBJECT

		public:
			Random1Widget(QWidget* parent = 0);
			~Random1Widget();


		private slots:
			void addButtonClicked();
			void builderThreadFinished();
			void updateProgress(int stepsCompleted, int totalSteps);


		private:
			//=====================  VARIABLES  =====================
			/*! Class that runs as a separate thread to create neuron group */
			Random1BuilderThread* builderThread;

			/*! Displays progress with the operation */
			QProgressDialog* progressDialog;

			/*! To enter a description of the neuron group */
			QLineEdit* descriptionEdit;

			/*! Combo to select neuron group that connection is from */
			QComboBox* fromCombo;

			/*! Combo to select neuron group that connection is to */
			QComboBox* toCombo;

			/*! The min value of weight range 1 */
			QLineEdit* minWeightRange1Edit;

			/*! The max value of weight range 1 */
			QLineEdit* maxWeightRange1Edit;

			/*! The proportion of weights falling within weight range 1 */
			QLineEdit* weightRange1PercentEdit;

			/*! The min value of weight range 1 */
			QLineEdit* minWeightRange2Edit;

			/*! The max value of weight range 1 */
			QLineEdit* maxWeightRange2Edit;

			/*! The min value of delay */
			QLineEdit* minDelayEdit;

			/*! The max value of delay */
			QLineEdit* maxDelayEdit;

			/*! The probability of making a connection between two neurons */
			QLineEdit* connectionProbabilityEdit;

			/*! To enter the type of synapse */
			QComboBox* synapseTypeCombo;


			//=====================  METHODS  =======================
			void addNeuronGroups(QComboBox* combo);
			void addSynapseTypes(QComboBox* combo);
			void checkInput(QComboBox* combo, const QString& errorMessage);
			void checkInput(QLineEdit* inputEdit, const QString& errorMessage);
			unsigned int getNeuronGroupID(const QString& comboText);
			unsigned int getSynapseTypeID(const QString& comboText);

	};

}

#endif//RANDOM1WIDGET_H
