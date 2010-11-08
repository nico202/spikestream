#ifndef CUBOIDBUILDERTHREAD_H
#define CUBOIDBUILDERTHREAD_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"
#include "SpikeStreamThread.h"

//Qt includes
#include <QThread>


namespace spikestream {

	/*! Adds a neuron group to the current network, which automatically adds it to the database */
	class CuboidBuilderThread : public SpikeStreamThread {
		Q_OBJECT

		public:
			CuboidBuilderThread();
			~CuboidBuilderThread();
			void prepareAddNeuronGroups(const QString& name, const QString& description, QHash<QString, double>& paramMap);
			void run();

		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);

		private:
			//=======================  VARIABLES  ========================
			/*! The neuron group(s) being added.
				The key is the neuron type ID. */
			QHash<unsigned int, NeuronGroup*> newNeuronGroupMap;

			/*! The threshold of the percentages used to add the different neuron types.
				If a random number is above the threshold, then the neuron is added to the group.
				The algorithm starts with the  lowest threshold
				The key is the percentage; the value is the neuron type ID. */
			QMap<double, unsigned int> neuronTypePercentThreshMap;

			/*! Network Dao used by Network when in thread */
			NetworkDao* threadNetworkDao;

			/*! Archive Dao used by Network when in thread */
			ArchiveDao* threadArchiveDao;

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

			/*! Neuron groups to be added, stored as a list */
			QList<NeuronGroup*> newNeuronGroupList;

			/*! The total number of neurons to be added. */
			unsigned totalNumberOfNeurons;


			//=======================  METHODS  ==========================
			void addNeuronGroupsToDatabase();
			void addNeurons();
			void createNeuronGroups(const QString& name, const QString& description, QHash<QString, double>& paramMap);
			double getParameter(const QString& paramName, const QHash<QString, double>& paramMap);
			void printSummary();
			void storeParameters(const QHash<QString, double>& paramMap);
	};
}

#endif//CUBOIDBUILDERTHREAD_H
