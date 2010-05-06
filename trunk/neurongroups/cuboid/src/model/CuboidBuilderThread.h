#ifndef CUBOIDBUILDERTHREAD_H
#define CUBOIDBUILDERTHREAD_H

//SpikeStream includes
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"

//Qt includes
#include <QThread>


namespace spikestream {

	/*! Adds a neuron group to the current network, which automatically adds it to the database */
	class CuboidBuilderThread : public QThread {
		Q_OBJECT

		public:
			CuboidBuilderThread();
			~CuboidBuilderThread();
			QString getErrorMessage(){ return errorMessage; }
			unsigned int getNeuronGroupID();
			bool isError() { return error; }
			void prepareAddNeuronGroup(const NeuronGroupInfo& neurGrpInfo);
			void run();
			void stop() { stopThread = true; }

		signals:
			void progress(int stepsCompleted, int totalSteps);

		private:
			//=======================  VARIABLES  ========================
			/*! Holds information about the neuron group that is being added */
			NeuronGroupInfo neuronGroupInfo;

			/*! Neuron group being added */
			NeuronGroup* newNeuronGroup;

			/*! Used to cancel the operation */
			bool stopThread;

			/*! Set to true when an error has occurred */
			bool error;

			/*! Message associated with an error */
			QString errorMessage;

			/*! Starting x position of neurons to be added */
			int xStart;

			/*! Starting y position of neurons to be added */
			int yStart;

			/*! Starting z position of neurons to be added */
			int zStart;

			/*! Width of the neuron group to be added along the x axis */
			int width;

			/*! Length of the neuron group to be added along the y axis */
			int length;

			/*! Height of the neuron group to be added along the z axis */
			int height;

			/*! Spacing between neurons in the neuron group */
			int spacing;

			/*! Density of the neurons */
			double density;


			//=======================  METHODS  ==========================
			void addNeuronGroup();
			void clearError();
			void buildNeuronGroup();
			double getParameter(const QString& paramName);
			void setError(const QString& errorMessage);
			void storeParameters();

	};
}

#endif//CUBOIDBUILDERTHREAD_H
