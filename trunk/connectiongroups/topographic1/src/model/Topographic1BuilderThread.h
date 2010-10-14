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

			/*! Connection projects to a Gaussian sphere of neurons */
			static const int GAUSSIAN_SPHERE = 0;

			/*! Connection projects to a uniform sphere of neurons  */
			static const int UNIFORM_SPHERE = 1;

			/*! Connection projects to a uniform cube of neurons */
			static const int UNIFORM_CUBE = 2;


			/*! Delay varies with distance */
			static const int DELAY_WITH_DISTANCE = 0;

			/*! Delays are randomly selected from a range of values */
			static const int RANDOM_DELAY = 1;


			//=======================  METHODS  ==========================
			void addProjectiveConnections(Neuron* fromNeuron, NeuronGroup* toNeurGrp, Box& projBox);
			float getDelay(Neuron* fromNeuron, Neuron* toNeuron);
			double getNormalRandom();
			float getWeight();
			bool makeGaussianConnection(const Box& projectionBox, const Point3D neuronLocation);
			bool makeGaussianConnection(float radius, float distance);

	};
}

#endif//TOPOGRAPHIC1BUILDERTHREAD_H
