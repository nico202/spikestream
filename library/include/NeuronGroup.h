#ifndef NEURONGROUP_H
#define NEURONGROUP_H

//SpikeStream includes
#include "Box.h"
#include "NeuronGroupInfo.h"
#include "Neuron.h"
using namespace spikestream;

/*! Structure holding pointers to neurons. */
typedef QHash<unsigned int, Neuron*> NeuronMap;

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
			void clearNeurons();
			bool contains(unsigned int neurID);
			bool contains(unsigned int neurID, float x, float y, float z);
			Box getBoundingBox();
			unsigned int getID() { return info.getID(); }
			unsigned int getNeuronAtLocation(const Point3D& location);
			QHash<QString, double> getParameters() { return parameterMap; }
			double getParameter(const QString& key);
			unsigned int getStartNeuronID() { return startNeuronID; }
			NeuronGroupInfo getInfo() {return info;}
			QList<unsigned int> getNeuronIDs() { return neuronMap->keys(); }
			NeuronMap* getNeuronMap() { return neuronMap; }
			Point3D& getNeuronLocation(unsigned int neuronID);
			bool isLoaded() { return loaded; }
			void setLoaded(bool loaded) { this->loaded = loaded; }
			void setID(unsigned int id){ info.setID(id); }
			void setNeuronMap(NeuronMap* newMap);
			void setParameters(QHash<QString, double>& paramMap){ this->parameterMap = paramMap; }
			void setStartNeuronID(unsigned int id) { this->startNeuronID = id; }
			int size();


		private:
			//==========================  VARIABLES  =========================
			/*! Information about the neuron group copied from NeuronGroup
				table in SpikeStreamNetwork database */
			NeuronGroupInfo info;

			/*! Map linking neuron Ids to a position in the array.
				This map can be used to get a list of all the neuron ids in the
				group and also to get the position of an individual neuron. */
			NeuronMap* neuronMap;

			/*! Returns true if the state of the neuron map matches the database.
				This should be false if no neurons have been loaded and false
				if the neuron map is full of neurons with dummy ids */
			bool loaded;

			/*! The first and lowest neuron id in the group. Useful when you know that
				a neuron group has continuously increasing IDs. */
			unsigned int startNeuronID;

			/*! Map of parameters for the neuron group */
			QHash<QString, double> parameterMap;

			/*! Box enclosing the neuron group */
			Box boundingBox;

			/*! Flag to record when bounding box needs to be recalculated. */
			bool calculateBoundingBox;


			//====================  METHODS  ==========================
			unsigned getTemporaryID();
			void neuronGroupChanged();
			NeuronGroup(const NeuronGroup& connGrp);
			NeuronGroup& operator=(const NeuronGroup& rhs);
    };

}

#endif//NEURONGROUP_H

