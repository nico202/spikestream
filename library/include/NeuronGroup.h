#ifndef NEURONGROUP_H
#define NEURONGROUP_H

//SpikeStream includes
#include "Box.h"
#include "NeuronGroupInfo.h"
#include "Point3D.h"


namespace spikestream {

    class NeuronGroup {
	public:
	    NeuronGroup(const NeuronGroupInfo& info);
	    ~NeuronGroup();
	    NeuronGroupInfo getInfo() {return info;}
	    QHash<unsigned int, Point3D*>* getNeuronMap() { return neuronMap; }
	    Box getMinimumBoundingBox() { return boundingBox; }
	    bool neuronsLoaded();
	    int size(){ return info.getNumberOfNeurons(); }

	 private:
	    //==========================  VARIABLES  =========================
	    /*! Information about the neuron group copied from NeuronGroup
		table in SpikeStreamNetwork database */
	    NeuronGroupInfo info;

	    /*! Map linking neuron Ids to a position in the array.
		This map can be used to get a list of all the neuron ids in the
		group and also to get the position of an individual neuron. */
	    QHash<unsigned int, Point3D*>* neuronMap;

	    /*! The minimum box enclosing the group */
	    Box boundingBox;
    };

}

#endif//NEURONGROUP_H

