#include "NeuronGroup.h"
using namespace spikestream;

/*! Constructor */
NeuronGroup::NeuronGroup(const NeuronGroupInfo& info){
    this->info = info;
    neuronMap = new QHash<unsigned int, Point3D*>();
    loaded = false;
}


/*! Destructor */
NeuronGroup::~NeuronGroup(){
    if(neuronMap != NULL)
	delete neuronMap;
}


/*--------------------------------------------------------- */
/*-----                PUBLIC METHODS                 ----- */
/*--------------------------------------------------------- */









