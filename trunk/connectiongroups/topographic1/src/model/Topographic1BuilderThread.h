#ifndef RANDOM1BUILDERTHREAD_H
#define TOPOGRAPHIC1BUILDERTHREAD_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "AbstractConnectionBuilder.h"


namespace spikestream {

	/*! Adds a connection group with topographic connections to the current network,
		which automatically adds it to the database if this is appropriate. */
	class Topographic1BuilderThread : public AbstractConnectionBuilder {
		Q_OBJECT

		public:
			Topographic1BuilderThread();
			~Topographic1BuilderThread();

		protected:
			void buildConnectionGroup();
			void checkParameters();

		private:
			//=======================  VARIABLES  ========================
			/*! Total number of progress steps */
			int numberOfProgressSteps;

			double projWidth;
			double projLength;
			double projHeight;
			double ovWidth;
			double ovLength;
			double ovHeight;
			int projectionPosition;
			bool forwardReverse;
			int conPattern;
			double minWeight;
			double maxWeight;
			int delayType;
			double delayDistanceFactor;
			unsigned minDelay;
			unsigned maxDelay;
			double density;

			static const int GAUSSIAN_SPHERE = 0;
			static const int UNIFORM_SPHERE = 1;
			static const int UNIFORM_CUBE = 2;

			//=======================  METHODS  ==========================
			void addProjectiveConnections(Neuron* centreNeuron, NeuronGroup* toNeurGrp, Box& projBox);

	};
}

#endif//TOPOGRAPHIC1BUILDERTHREAD_H
