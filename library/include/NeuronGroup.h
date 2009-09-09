#ifndef NEURONGROUP_H
#define NEURONGROUP_H

//SpikeStream includes
#include "NeuronGroupInfo.h"
#include "Point3D.h"
using namespace spikestream;

typedef QHash<unsigned int, Point3D*> NeuronMap;

namespace spikestream {

    class NeuronGroup {
	public:
	    NeuronGroup(const NeuronGroupInfo& info);
	    ~NeuronGroup();
	    unsigned int getID() { return info.getID(); }
	    NeuronGroupInfo getInfo() {return info;}
	    NeuronMap* getNeuronMap() { return neuronMap; }
	    bool isLoaded() { return loaded; }
	    void setLoaded(bool loaded) { this->loaded = loaded; }
	    void setID(unsigned int id){ info.setID(id); }

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

	   //====================  METHODS  ==========================
	   NeuronGroup(const NeuronGroup& connGrp);
	   NeuronGroup& operator=(const NeuronGroup& rhs);
    };

}

#endif//NEURONGROUP_H

