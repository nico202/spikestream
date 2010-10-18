#ifndef NEURONGROUPDIALOG_H
#define NEURONGROUPDIALOG_H

//SpikeStream includes
#include "NeuronGroupInfo.h"

//Qt includes
#include <QDialog>
#include <QLineEdit>

namespace spikestream {

	/*! Dialog for editing the properties of a neuron group. */
	class NeuronGroupDialog : public QDialog {
		Q_OBJECT

		public:
			NeuronGroupDialog(const NeuronGroupInfo& neurGrpInfo, QWidget* parent=0);
			~NeuronGroupDialog();


		private slots:
			void okButtonPressed();
			void cancelButtonPressed();


		private:
			//====================  VARIABLES ====================
			/*! Where user enters the name of the neuron group */
			QLineEdit* nameLineEdit;

			/*! Where user enters a description of the neuron group */
			QLineEdit* descLineEdit;

			/*! Information about the neuron group being edited. */
			NeuronGroupInfo neuronGroupInfo;


			//====================  METHODS  ====================
			void buildGUI();
			QString getName();
			QString getDescription();
	};
}

#endif//NEURONGROUPDIALOG_H
