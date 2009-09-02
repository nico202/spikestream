#ifndef NEURONGROUP_H
#define NEURONGROUP_H

namespace SpikeStream {

    class NeuronGroup {
	public:
	    NeuronGroup(NeuronGroupInfo* info);
	    NeuronGroupInfo* getInfo();
	    bool isLoaded();



	 private:
	    //==========================  VARIABLES  =========================
	    /*! Information about the neuron group copied from NeuronGroup
		table in SpikeStreamNetwork database */
	    NeuronGroupInfo* info;

	    /*! ID of the first neuron. This is used for rapid access to X, Y, Z
				coordinates.*/
	    unsigned int startNeuronID;
	    unsigned int *neuronIDArray;
	    float *xPosArray;
	    float *yPosArray;
	    float *zPosArray;
	    unsigned short neuronType;
	    unsigned int numberOfNeurons;
	    QString name;
	    unsigned int width;
	    unsigned int length;
	    unsigned int depth;
	    unsigned int neuronSpacing;
	    int xPos;
	    int yPos;
	    int zPos;
	    ClippingVolume clippingVolume;

    };

}

#endif//NEURONGROUP_H

