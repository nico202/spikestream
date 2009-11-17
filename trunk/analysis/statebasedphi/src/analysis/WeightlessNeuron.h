#ifndef WEIGHTLESSNEURON_H
#define WEIGHTLESSNEURON_H

//Qt includes
#include <QHash>

namespace spikestream {

    /*! I think this is an array where each number represents the Hamming distance
	between XORed bytes. So for example, if the two bytes 10000111 and 00000111
	are XORed, you get the number 10000000, or 128, which should result in 1 when
	looked up in the array.
	FIXME: CHECK THIS ARRAY */
    byte lookup[] =
    {
      0,1,1,2, 1,2,2,3, 1,2,2,3, 2,3,3,4,
      1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
      1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
      2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
      1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
      2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
      2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
      3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
      1,2,2,3, 2,3,3,4, 2,3,3,4, 3,4,4,5,
      2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
      2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
      3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
      2,3,3,4, 3,4,4,5, 3,4,4,5, 4,5,5,6,
      3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
      3,4,4,5, 4,5,5,6, 4,5,5,6, 5,6,6,7,
      4,5,5,6, 5,6,6,7, 5,6,6,7, 6,7,7,8
    };

    typedef unsigned char byte;


    class WeightlessNeuron {
	public:
	    WeightlessNeuron();
	    ~WeightlessNeuron();


	private:
	    //===================  VARIABLES  ===================
	    /*! Map linking each position in the pattern string with
		a neuron id. Key is the connected neuron id; value is the index in the pattern string. */
	    QHash<unsigned int, unsigned int> connectionMap;

	    /*! List of training arrays */
	    QList<byte*> trainingData;

	    /*! The Hamming distance */
	    int hammingDistance;

    };

}

#endif//WEIGHTLESSNEURON_H

