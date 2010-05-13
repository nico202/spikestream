#ifndef NEMOWIDGET_H
#define NEMOWIDGET_H

//SpikeStream includes
#include "NemoWrapper.h"

//Qt includes
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolBar>
#include <QWidget>

namespace spikestream {

	/*! Graphical interface for the plugin wrapping the Nemo spiking neural simulator */
	class NemoWidget : public QWidget {
		Q_OBJECT

		public:
			NemoWidget(QWidget* parent = 0);
			~NemoWidget();

		private slots:
			void loadSimulation();
			void setParameters();
			void simulationAdvanceOccurred();
			void simulationRateChanged(int comboIndex);
			void startSimulation();
			void stepSimulation();
			void stopSimulation();
			void unloadSimulation();

		private:
			//========================  VARIABLES  ========================
			/*! Wrapper for Nemo library */
			NemoWrapper* nemoWrapper;

			/*! Button for loading simulation */
			QPushButton* loadButton;

			/*! Button for unloading simulation */
			QPushButton* unloadButton;

			/*! Button that launches dialog to edit the parameters */
			QPushButton* parametersButton;

			/*! Tool bar with transport controls for loading, playing, etc. */
			QToolBar toolBar;

			/*! For user to enter a description of the archive */
			QLineEdit* archiveDescriptionEdit;

			/*! Sets the description of the archive */
			QPushButton* setArchiveDescriptionButton;

			/*! Rate of the simulation */
			QComboBox* simulationRateCombo;

			/*! Time step of the simulation */
			QLabel* timeStepLabel;


			//=======================  METHODS  =========================
			void checkForErrors();
	};

}

#endif//NEMOWIDGET_H

