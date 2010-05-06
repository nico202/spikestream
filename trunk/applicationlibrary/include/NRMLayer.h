#ifndef NRMLAYER_H
#define NRMLAYER_H


//Other imports
#include <string>
using namespace std;

namespace nrm {

    /*! Holds information that is common to all types of layers.
		    Neural layer and input layer extend this class. */
    class NRMLayer {
	    public:
		    //==========================  METHODS  ==========================
		    NRMLayer();
		    virtual ~NRMLayer();
		    int getSize();
		    virtual void print() = 0;

		    //=========================  VARIABLES  =========================
		    unsigned int width;
		    unsigned int height;
		    int colPlanes;
		    int x;
		    int y;
		    string frameName;
		    int frameNum;

		    /*! The ID of the corresponding SpikeStream neuron group.*/
		    unsigned int spikeStreamID;

		    /*! The type of the layer */
		    int type;

		    /*! Input layer type */
		    static const int NRM_INPUT_LAYER = 1;

		    /*! Neural layer type */
		    static const int NRM_NEURAL_LAYER = 2;

    };

}

#endif // NRMLAYER_H
