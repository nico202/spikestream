#ifndef NEURONGROUP_H
#define NEURONGROUP_H

//SpikeStream includes
#include "Box.h"
#include "NeuronGroupInfo.h"
#include "Neuron.h"
using namespace spikestream;

//Qt includes
#include <stdint.h>
#include <QMap>

/*! Structure holding pointers to neurons. */
typedef QHash<unsigned int, Neuron*> NeuronMap;

/*! Iterator over positions in the group */
typedef QMap<uint64_t, Neuron*>::iterator NeuronPositionIterator;

/*! An iterator for working through all of the neurons in the group. */
typedef NeuronMap::iterator NeuronIterator;

namespace spikestream {

	/*! A group of neurons. */
    class NeuronGroup {
		public:
			NeuronGroup(const NeuronGroupInfo& info);
			~NeuronGroup();
			void addLayer(int width, int height, int xPos, int yPos, int zPos);
			Neuron* addNeuron(float xPos, float yPos, float zPos);
			NeuronMap::iterator begin() { return neuronMap->begin(); }
			NeuronMap::iterator end() { return neuronMap->end(); }
			NeuronPositionIterator positionBegin();
			NeuronPositionIterator positionEnd();
			void buildPositionMap();
			void clearNeurons();
			bool contains(unsigned int neurID);
			bool contains(unsigned int neurID, float x, float y, float z);
			Box getBoundingBox();
			unsigned int getID();
			NeuronGroupInfo getInfo() {return info;}
			Neuron* getNearestNeuron(const Point3D& point);
			unsigned int getNeuronIDAtLocation(const Point3D& location);
			QList<unsigned int> getNeuronIDs() { return neuronMap->keys(); }
			Point3D& getNeuronLocation(unsigned int neuronID);
			NeuronMap* getNeuronMap() { return neuronMap; }
			QList<Neuron*> getNeurons(const Box& box);
			unsigned getNeuronTypeID();
			double getParameter(const QString& key);
			QHash<QString, double> getParameters() { return parameterMap; }
			static Point3D getPointFromPositionKey(uint64_t positionKey);
			static uint64_t getPositionKey(int xPos, int yPos, int zPos);
			unsigned int getStartNeuronID() { return startNeuronID; }
			bool isWeightless();
			bool parametersSet();
			void setID(unsigned int id);
			void setDescription(const QString& description);
			void setName(const QString& name);
			void setNeuronMap(NeuronMap* newMap);
			void setParameters(QHash<QString, double>& paramMap);
			void setStartNeuronID(unsigned int id) { this->startNeuronID = id; }
			int size();


		private:
			//==========================  VARIABLES  =========================
			/*! Information about the neuron group copied from NeuronGroup
				table in SpikeStreamNetwork database */
			NeuronGroupInfo info;

			/*! Map linking neuron Ids to Neuron classes.
				This map can be used to get a list of all the neuron ids in the
				group and also to get the position of an individual neuron. */
			NeuronMap* neuronMap;

			/*! Map linking a 64 bit number describing neuron position
				with pointer to neuron class. This map enables iteration through
				neuron group in a geometrically ordered way. */
			QMap<uint64_t, Neuron*> neuronPositionMap;

			/*! Flag recording whether position map has been built.
				Position map takes processor power and memory and is rarely used,
				so it is generated on demand when an iterator to it is requested. */
			bool positionMapBuilt;

			/*! The first and lowest neuron id in the group. Useful when you know that
				a neuron group has continuously increasing IDs. */
			unsigned int startNeuronID;

			/*! Map of parameters for the neuron group */
			QHash<QString, double> parameterMap;

			/*! Box enclosing the neuron group */
			Box boundingBox;

			/*! Flag to record when bounding box needs to be recalculated. */
			bool calculateBoundingBox;

			/*! Static counter used to generate neuron IDs that are unique for an instance of the application.
				 Used for prototyping networks. */
			static unsigned neuronIDCounter;


			//====================  METHODS  ==========================
			unsigned getTemporaryID();
			void loadDefaultParameters();
			void neuronGroupChanged();
			NeuronGroup(const NeuronGroup& connGrp);
			NeuronGroup& operator=(const NeuronGroup& rhs);
    };

}

#endif//NEURONGROUP_H

