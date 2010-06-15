#ifndef NEURONGROUPWIDGET_H
#define NEURONGROUPWIDGET_H

//SpikeStream includes
#include "NeuronGroupModel.h"

//Qt includes
#include <QProgressDialog>
#include <QPushButton>
#include <QWidget>

namespace spikestream {

	/*! Holds the table displaying information about the neuron groups in the current network.
		Includes buttons to add and delete neuron groups. */
	class NeuronGroupWidget : public QWidget {
		Q_OBJECT

		public:
			NeuronGroupWidget(QWidget* parent = 0);
			~NeuronGroupWidget();


		private slots:
			void addNeurons();
			void deleteSelectedNeurons();
			void networkChanged();
			void networkTaskFinished();


		private:
			//========================  VARIABLES  ========================
			/*! Model for display of list of neuron groups */
			NeuronGroupModel* neuronGroupModel;

			/*! Button for adding neurons */
			QPushButton* addNeuronsButton;

			/*! Button for deleting neurons */
			QPushButton* deleteButton;

			/*! Provides feedback with progress deleting neurons */
			QProgressDialog* progressDialog;
	};

}

#endif//NEURONGROUPWIDGET_H


