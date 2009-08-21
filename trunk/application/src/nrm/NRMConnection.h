#ifndef NRMCONNECTION_H
#define NRMCONNECTION_H

//#include "NRMNetwork.h"
//Qt includes
#include <QList>

/*! Connection parameters
	NOTE: The size of this struct is critical because it is used in the NRM loading */
struct conType {
	bool randomCon;
	bool fullCon;
	bool spatialAlignment;
	bool HFlipped;
	bool VFlipped;
	bool globalMap;
	bool iconicMap;
	bool segmentedMap;
	bool gazeMap;
	bool tileMap;
	unsigned int colScheme;
	unsigned int numCons;
	unsigned int sWidth;  // Spatial parameters
	unsigned int sHeight;
	unsigned int sLeft;
	unsigned int sTop;
	unsigned int rWidth;  // Receptive Field parameters
	unsigned int rHeight;
	bool segRev;
};


/*! Class holding details about the connection between two layers */
class NRMConnection {
	public:
		//===========================  METHODS  ============================
		NRMConnection(void* nw);
		~NRMConnection();
		void createConnections();
		QList<unsigned int> getNeuronConnections(unsigned int neurNum);
		void print();
		void printNeuronConnections(unsigned int neurNum);
		void reset();
		void resetParameters();

		//==========================  VARIABLES  ==========================
		int x1;
		int y1;
		int x2;
		int y2;
		int x3;
		int y3;
		int x4;
		int y4;
		int x5;
		int y5;
		int x6;
		int y6;
		bool selected;
		int srcLayerId;
		int srcObjectType;
		int destLayerId;
		int destObject;
		int destObjectType;
		int connection;

		/*! Connection parameters */
		conType conParams;

		NRMConnection* prev;
		NRMConnection* next;

	private:
		//=========================  VARIABLES  =========================
		/*! Network of which this connection is a part */
		void* parentNetwork;

		/*! Array holding the details about the connection between the source and
			destination neurons */
		short* wRandCons;

		//==========================  METHODS  ==========================
		void printParameters();

};

#endif // NRMCONNECTION_H
