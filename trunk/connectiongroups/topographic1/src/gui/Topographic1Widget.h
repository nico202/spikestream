#ifndef TOPOGRAPHIC1WIDGET_H
#define TOPOGRAPHIC1WIDGET_H

//SpikeStream includes
#include "AbstractConnectionWidget.h"

//Qt includes
#include <QComboBox>
#include <QLineEdit>
#include <QProgressDialog>
#include <QWidget>

namespace spikestream {

	/*! Gathers information for creating a topographic connection pattern between two neuron groups. */
	class Topographic1Widget : public AbstractConnectionWidget {
		Q_OBJECT

		public:
			Topographic1Widget(QWidget* parent = 0);
			~Topographic1Widget();

		protected:
			bool checkInputs();
			ConnectionGroupInfo getConnectionGroupInfo();

		private slots:
			void delayTypeChanged(int indx);

		private:
			//=====================  VARIABLES  =====================
			/*! To enter a description of the neuron group */
			QLineEdit* descriptionEdit;

			/*! Combo to select FROM neuron group */
			QComboBox* fromNeuronGrpCombo;

			/*! Combo to select TO neuron group  */
			QComboBox* toNeuronGrpCombo;

			/*! Width of each projection on the X axis */
			QLineEdit* projectionWidthEdit;

			/*! Width of each projection on the Y axis */
			QLineEdit* projectionLengthEdit;

			/*! Width of each projection on the Z axis */
			QLineEdit* projectionHeightEdit;

			/*! Overlap on X axis */
			QLineEdit* overlapWidthEdit;

			/*! Overlap on Y axis */
			QLineEdit* overlapLengthEdit;

			/*! Overlap on Z axis */
			QLineEdit* overlapHeightEdit;

			/*! Position of projections relative to destination layer */
			QComboBox* positionCombo;

			/*! Direction of the connection */
			QComboBox* forRevCombo;

			/*! Connection pattern - Gaussian, uniform sphere, uniform cube. */
			QComboBox* connectionPatternCombo;

			/*! Probability that connection is created */
			QLineEdit* densityEdit;

			/*! Minimum weight */
			QLineEdit* minWeightEdit;

			/*! Maximum weight */
			QLineEdit* maxWeightEdit;

			/*! Controls whether delay matches distances or falls within a range.*/
			QComboBox* delayTypeCombo;\

			/*! When delay matches distance, this is the factor by which the delay is multiplied. */
			QLineEdit* delayDistanceFactorEdit;

			/*! When the delay is in a range, this is the minimum of the range */
			QLineEdit* minDelayEdit;

			/*! When the delay is in a range, this is the maximum of the range. */
			QLineEdit* maxDelayEdit;

			/*! Type of synapse */
			QComboBox* synapseTypeCombo;

			/*! Vertical spacing between lines */
			static const int vSpacing = 5;

			/*! Main vertical box of widget */
			QVBoxLayout* mainVBox;

			//=====================  METHODS  ======================
			void addInputWidget(QWidget* widget, QHBoxLayout* layout, QString label, bool limitWidth = false, bool lastWidget = false);
			void buildGUI(QVBoxLayout* mainVBox);
			void fillConnectionPatternCombo();
			void fillPositionCombo();

	};

}

#endif//TOPOGRAPHIC1WIDGET_H
