#ifndef NRMLAYER_H
#define NRMLAYER_H


//Other imports
#import <string>
using namespace std;


/*! Holds information that is common to all types of layers.
		Neural layer and input layer extend this class. */
class NRMLayer {
	public:
		//==========================  METHODS  ==========================
		NRMLayer();
		virtual ~NRMLayer();
		unsigned int getSize();
		virtual void print() = 0;

		//=========================  VARIABLES  =========================
		unsigned int width;
		unsigned int height;
		int colPlanes;
		int x;
		int y;
		string frameName;
		int frameNum;
};

#endif // NRMLAYER_H
