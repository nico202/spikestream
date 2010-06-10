//SpikeStream includes
#include "NRMConnection.h"
#include "NRMRandom.h"
#include "NRMNetwork.h"
#include "NRMLayer.h"
#include "NRMException.h"
#include "NRMConstants.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

/*! Constructor */
NRMConnection::NRMConnection(void* nw){
	//Store reference to parent network
	parentNetwork = nw;

	//Set all variables into a known state to help debug loading
	reset();
}


/*! Destructor */
NRMConnection::~NRMConnection(){
	delete [] wRandCons;
}


/*! Connections are not saved by NRM, which recreates them using the same random seed each time the network is
	built. So a lot of crufty NRM code had to be pasted in here to reconstruct wRandCons, which contains the list of
	connections. In wRandCons the offset is the source neuron number multiplied by the number of connections per neuron in the
	connection path. This source neuron will then have numCons numbers in wRandCons, which are the numbers of the neurons
	in the destination layer. */
void NRMConnection::createConnections(){
	//Local variables
	unsigned int segNumH;
	unsigned int segNumV;
	unsigned int conListSize, fullCount, count;
	int x, y, p, xx, yy, off;
	//bool bigNet = false;
	//int netType = SMALL_NET + spreading;

	/* Local copies of current and destination layer
		WARNING: This follows the NRM conventions, but note that the connections are from
			the destination to the current layer. */
	NRMLayer* curLayer = ((NRMNetwork*)parentNetwork)->getLayerById(srcLayerId, srcObjectType);
	NRMLayer* destLayer = ((NRMNetwork*)parentNetwork)->getLayerById(destLayerId, destObjectType);

	//Set seed for random number generator
	NRMRandom::setSeed(srcLayerId + 1);

	//Only support connection to layers less than 65536 neurons
	if ( destLayer->getSize() > 0x10000 ) {
		throw NRMException("This loader does not support destination layers with 65536 neurons or more");
	}

	//Size of connection list for connection path
	if ( conParams.gazeMap ) {
		segNumH = conParams.sWidth;
		segNumV = conParams.sHeight;
		conListSize = curLayer->getSize() * conParams.numCons * segNumH * segNumV;
	}
	else if ( conParams.segRev ) {
		segNumH = destLayer->width / curLayer->width;
		segNumV = destLayer->height / curLayer->height;
		conListSize = curLayer->getSize() * conParams.numCons * segNumH * segNumV;
	}
	else{
		conListSize = curLayer->getSize() * conParams.numCons;
	}

	//Create array to hold the list of ids that each neuron in the source layer connects to
	try{
		wRandCons = new short[conListSize];
	}
	catch (exception ex){
		throw NRMException("Memory cannot be allocated for connections array.");
	}

	//Set up connections for Global Mapping
	if ( conParams.globalMap ) {
		if ( conParams.randomCon ) {//Setup random connections
			if ( conParams.spatialAlignment ) { //Setup random Spatially aligned connections
				int randX, randY;
				for (unsigned int i = 0 ; i < conListSize; i++ ) {
					randX = NRMRandom::getRandNum(conParams.sWidth) + conParams.sLeft;
					randY = NRMRandom::getRandNum(conParams.sHeight) + conParams.sTop;
					wRandCons[i] = randX + randY * destLayer->width;
				}
			}
			else {//Setup normal random connections
				for (unsigned int i = 0 ; i < conListSize; i++ ){
					wRandCons[i] = NRMRandom::getRandNum(destLayer->getSize());
				}
			}
		}
		else {   //Setup connections for full global connected
			if ( conParams.spatialAlignment ) { //Setup full Spatially aligned connections
				off = conParams.sTop * destLayer->width + conParams.sLeft;
				count = 0;
				for (int nn = 0; nn <curLayer->getSize(); nn++ ) {  // for all neurons
					fullCount = 0;
					for (unsigned int c = 0; c < conParams.numCons; c++ ) {
						if ( destLayer->colPlanes == 1 )
							p = fullCount++;
						else {
							if ( c && !(c % 8) )
								fullCount++;
							p = fullCount;
						}
						yy = p / conParams.sWidth;
						xx = p - yy * conParams.sWidth;
						p = off + (yy * destLayer->width + xx);
						wRandCons[count++] = p;
					}
				}
			}
			else {//Setup normal global full connections
				if ( destLayer->colPlanes == 1 ) {  // B&W
					count = 0;
					for (unsigned int i = 0 ; i < conListSize; i++ ) {
						wRandCons[i] = count++;
						if ( count == conParams.numCons )
							count = 0;
					}
				}
				else {  //Colour
					count = 0;
					int cycle = 0;
					for (unsigned int i = 0 ; i < conListSize; i++ ) {
						wRandCons[i] = cycle;
						count++;
						if ( !(count % 8) )
							cycle++;
						if ( count == conParams.numCons ) {
							count = 0;
							cycle = 0;
						}
					}
				}
			}
		}
	}

	//Setup connections for iconic Mapping
	if ( conParams.iconicMap ) {
		//coordinates of neuron
		count = 0;
		fullCount = 0;
		// no wrapping
		for (int nn = 0; nn <curLayer->getSize(); nn++ ) {  // for all neurons
			if ( conParams.spatialAlignment ) { // do spatial aligned iconic
				off = conParams.sTop * destLayer->width + conParams.sLeft;
				for (unsigned int c = 0; c < conParams.numCons; c++ ) {
					p = fullCount;
					yy = p / curLayer->width;
					xx = p - yy * curLayer->width;

					if ( conParams.sWidth > curLayer->width )
						xx = (conParams.sWidth * (xx + xx + 1)) / (curLayer->width + curLayer->width);
					else if ( conParams.sWidth < curLayer->width )
						xx = (xx * conParams.sWidth) / curLayer->width;

					if ( conParams.sHeight > curLayer->height )
						yy = (conParams.sHeight * (yy + yy + 1)) / (curLayer->height + curLayer->height);
					else if ( conParams.sHeight < curLayer->height )
						yy = (yy * conParams.sHeight) / curLayer->height;

					p = off + (yy * destLayer->width + xx);

					wRandCons[count++] = p;
				}
				fullCount++;
			}
			else { // not spatial
				y =  nn / curLayer->width;
				x =  nn - y * curLayer->width;
				if ( destLayer->width > curLayer->width )
					x = (destLayer->width * (x + x + 1)) / (curLayer->width + curLayer->width) ;
				else if ( destLayer->width < curLayer->width )
					x = (x * destLayer->width) / curLayer->width;

				if ( destLayer->height > curLayer->height )
					y = (destLayer->height * (y + y + 1)) / (curLayer->height + curLayer->height);
				else if ( destLayer->height < curLayer->height )
					y = (y * destLayer->height) / curLayer->height;

				x -= conParams.rWidth / 2;

				if ( x < 0 )
					x = 0;
				else if ( x + conParams.rWidth > destLayer->width)
					x = destLayer->width - 1 - conParams.rWidth;
				//   	         		x = destLayer->width - conParams.rWidth;


				y -= conParams.rHeight / 2;

				if ( y < 0 )
					y = 0;
				else if ( y + conParams.rHeight > destLayer->height)
					y = destLayer->height - 1 - conParams.rHeight;
				//   	         		y = destLayer->height - conParams.rHeight;

				if ( ( conParams.VFlipped ) && ( conParams.HFlipped ) )
					off =curLayer->getSize() - (y * destLayer->width + x) - 1;
				else if ( conParams.VFlipped )
					off = (destLayer->height - y - 1) * destLayer->width + x;
				else if ( conParams.HFlipped )
					off = y * destLayer->width + destLayer->width - x - 1;
				else
					off = y * destLayer->width + x;

				fullCount = 0;
				unsigned int c = 0;
				while ( c < conParams.numCons ) {
					//	            for ( int c = 0; c < conParams.numCons; c++ ) {
					if ( conParams.fullCon && ( destLayer->colPlanes == 1 ) )
						p = fullCount++;
					else if ( conParams.fullCon && ( destLayer->colPlanes > 1) ) {
						if ( c && !(c % 8) )
							fullCount++;
						p = fullCount;
					}
					else
						p = NRMRandom::getRandNum(conParams.rHeight * conParams.rWidth);
					yy = p / conParams.rWidth;
					xx = p - yy * conParams.rWidth;

					if ( ( conParams.VFlipped ) && ( conParams.HFlipped ) )
						p = off - (yy * destLayer->width + xx);
					else if ( conParams.VFlipped )
						p = off - (yy * destLayer->width + xx);
					else if ( conParams.HFlipped )
						p = off + (yy * destLayer->width + xx);
					else {
						p = off + (yy * destLayer->width + xx);
					}
					wRandCons[count++] = p;
					c++;
				}
			}
		}
	}

	//Segmented map connections
	if ( conParams.segmentedMap ) {
		if ( conParams.segRev ) {
			int segPos;
			count = 0;
			for (unsigned int y = 0; y < curLayer->height; y++) {
				for (unsigned int x = 0; x < curLayer->width; x++ ) {  // for all neurons
					for (unsigned int h = 0; h < segNumH; h++ ) {
						for (unsigned int w = 0; w < segNumV; w++ ) {  // for all segments
							segPos = (h * destLayer->width * curLayer->height) + (w * curLayer->width);
							p = segPos + (y * destLayer->width) + x;
							for (unsigned int c = 0; c < conParams.numCons; c++ ) {
								wRandCons[count++] = p;
							}
						}
					}
				}
			}
		}
		else if ( ((NRMNeuralLayer*)curLayer)->altParam2 < 1 ) {
			count = 0; fullCount = 0;
			for (int nn = 0; nn <curLayer->getSize(); nn++ ) {  // for all neurons
				for (unsigned int c = 0; c < conParams.numCons; c++ ) {
					p = fullCount % destLayer->width +
						( (fullCount / curLayer->height) % destLayer->height ) * destLayer->width;

					y = p / destLayer->width;
					x = p - y * destLayer->width;
					p = (y * destLayer->width + x);
					wRandCons[count++] = p;
				}
				fullCount++;
			}
		}
	}

	if ( conParams.tileMap ) {
		int xOff, yOff;
		int xTileSize = destLayer->width / curLayer->width;
		int yTileSize = destLayer->height / curLayer->height;
		fullCount = 0;
		count = 0;
		if ( conParams.randomCon ) {
			for (int nn = 0; nn <curLayer->getSize(); nn++ ) {  // for all neurons
				for (unsigned int c = 0; c < conParams.numCons; c++ ) {
					// find the top left corner of each tiled mapping on the destination object
					yOff = yTileSize * ( fullCount / curLayer->width );
					xOff = xTileSize * ( fullCount - ( fullCount /curLayer->width ) *curLayer->width );
					p = ( NRMRandom::getRandNum(xTileSize) + xOff ) + ( NRMRandom::getRandNum(yTileSize) + yOff ) * destLayer->width;
					wRandCons[count++] = p;
				}
				fullCount++;
			}
		}
		else if ( conParams.fullCon ) {
			int xPos, yPos, pos;
			int xTileSize = destLayer->width /curLayer->width;
			int yTileSize = destLayer->height / curLayer->height;
			for (int nn = 0; nn <curLayer->getSize(); nn++ ) {  // for all neurons
				pos = 0;
				for (unsigned int c = 0; c < conParams.numCons; c++ ) {
					// find the top left corner of each tiled mapping on the destination object
					yOff = yTileSize * ( fullCount /curLayer->width );
					xOff = xTileSize * ( fullCount - ( fullCount /curLayer->width ) *curLayer->width );

					// Note the following code cycles through part of a single tile
					if ( conParams.colScheme == WHITE_COLS )  {
						if ( c )
							pos++;
					}
					else
						if ( !(c % 8) && c)
							pos++;

					yPos = pos / (xTileSize);
					xPos = pos - yPos * (xTileSize);

					p = ( xPos + xOff ) + ( yPos + yOff ) * destLayer->width;

					wRandCons[count++] = p;

				}
				fullCount++;
			}
		}
	}

	// segmented gaze connections required
	if ( conParams.gazeMap ) {
		int adj = 0;
		//      	if ( altParam2 )
		//         	adj = 1;
		int stepH =curLayer->width / conParams.sWidth;
		int stepV = curLayer->height / conParams.sHeight;
		int xLeft, yTop;
		for (unsigned int nY = 0; nY < curLayer->height; nY++ ) {
			for (unsigned int nX = 0; nX <curLayer->width; nX++ ) {
				for (unsigned int segY = 0; segY < segNumV; segY++ ) {
					yTop = ((segNumV - segY - 1) * stepV + nY - adj) * destLayer->width;
					for (unsigned int segX = 0; segX < segNumH; segX++ ) {
						xLeft = (segNumH - segX - 1) * stepH + nX - adj;
						p = yTop + xLeft;
						if ( p < 0 || p > (int)(destLayer->width * destLayer->height - 1) )
							p = 0;
						for (unsigned int c = 0; c < conParams.numCons; c++ ) {
							wRandCons[count++] = p;
						}
					}
				}
			}
		}
	}
}


/*! Returns a list of the connections made by the specified neuron within this connection path */
QList<unsigned int> NRMConnection::getNeuronConnections(unsigned int neurNum){
	//Check neuron number is in the source layer
	NRMLayer* curLayer = ((NRMNetwork*)parentNetwork)->getLayerById(srcLayerId, srcObjectType);
	if(neurNum >= (unsigned int)curLayer->getSize())
		throw NRMException ("Requested neuron number is out of range: ", neurNum);

	//Add the connections for this neuron to a list
	QList<unsigned int> tmpList;
	unsigned int offset = neurNum * conParams.numCons;
	for (unsigned int i = offset; i < offset + conParams.numCons; ++i ) {
		tmpList.append(wRandCons[i]);
	}
	return tmpList;
}


/*! Sets all of the class variables into a known ideally invalid state */
void NRMConnection::reset(){
	x1 = -1;
	y1 = -1;
	x2 = -1;
	y2 = -1;
	x3 = -1;
	y3 = -1;
	x4 = -1;
	y4 = -1;
	x5 = -1;
	y5 = -1;
	x6 = -1;
	y6 = -1;
	selected = false;
	srcLayerId = -1;
	srcObjectType = -1;
	destLayerId = -1;
	destObject = -1;
	destObjectType = -1;
	connection = -1;

	resetParameters();
}


/*! Sets all of the parameters into a known ideally invalid state */
void NRMConnection::resetParameters(){
	conParams.randomCon = false;
	conParams.fullCon = false;
	conParams.spatialAlignment = false;
	conParams.HFlipped = false;
	conParams.VFlipped = false;
	conParams.globalMap = false;
	conParams.iconicMap = false;
	conParams.segmentedMap = false;
	conParams.gazeMap = false;
	conParams.tileMap = false;
	conParams.colScheme = 0;
	conParams.numCons = 0;
	conParams.sWidth = 0;  // Spatial parameters
	conParams.sHeight = 0;
	conParams.sLeft = 0;
	conParams.sTop = 0;
	conParams.rWidth = 0;  // Receptive Field parameters
	conParams.rHeight = 0;
	conParams.segRev = false;
}


/*! Prints out information about the connection */
void NRMConnection::print(){
	cout<<"=============== NRM Connection ============"<<endl;
		cout<<"x1: "<<x1<<endl;
		cout<<"y1: "<<y1<<endl;
		cout<<"x2: "<<x2<<endl;
		cout<<"y2: "<<y2<<endl;
		cout<<"x3: "<<x3<<endl;
		cout<<"y3: "<<y3<<endl;
		cout<<"x4: "<<x4<<endl;
		cout<<"y4: "<<y4<<endl;
		cout<<"x5: "<<x5<<endl;
		cout<<"y5: "<<y5<<endl;
		cout<<"x6: "<<x6<<endl;
		cout<<"y6: "<<y6<<endl;
		cout<<"selected: "<<selected<<endl;
		cout<<"srclayerId: "<<srcLayerId<<endl;
		cout<<"srclayerObjType: "<<srcObjectType<<endl;
		cout<<"destLayerId: "<<destLayerId<<endl;
		cout<<"destObject: "<<destObject<<endl;
		cout<<"destObjectType: "<<destObjectType<<endl;
		cout<<"connection??: "<<connection<<endl;
		printParameters();
}

/*! Prints out the connections to the specified neuron */
void NRMConnection::printNeuronConnections(unsigned int neurNum){
	QList<unsigned int> neurConnList = getNeuronConnections(neurNum);
	for(QList<unsigned int>::iterator iter = neurConnList.begin(); iter != neurConnList.end(); ++iter){
		cout<<*iter<<",";
	}
	cout<<endl;
}


/*! Prints out the parameters of the connection */
void NRMConnection::printParameters(){
	cout<<"=================== Connection Parameters ==============="<<endl;
	cout<<"randomCon: "<<conParams.randomCon<<endl;
	cout<<"fullCon: "<<conParams.fullCon<<endl;
	cout<<"spatialAlignment: "<<conParams.spatialAlignment<<endl;
	cout<<"HFlipped: "<<conParams.HFlipped<<endl;
	cout<<"VFlipped: "<<conParams.VFlipped<<endl;
	cout<<"globalMap: "<<conParams.globalMap<<endl;
	cout<<"iconicMap: "<<conParams.iconicMap<<endl;
	cout<<"segmentedMap: "<<conParams.segmentedMap<<endl;
	cout<<"gazeMap: "<<conParams.gazeMap<<endl;
	cout<<"tileMap: "<<conParams.tileMap<<endl;
	cout<<"colScheme: "<<conParams.colScheme<<endl;
	cout<<"numCons: "<<conParams.numCons<<endl;
	cout<<"sWidth: "<<conParams.sWidth<<endl;  // Spatial conParamseters
	cout<<"sHeight: "<<conParams.sHeight<<endl;
	cout<<"sLeft: "<<conParams.sLeft<<endl;
	cout<<"sTop: "<<conParams.sTop<<endl;
	cout<<"rWidth: "<<conParams.rWidth<<endl;  // Receptive Field conParamseters
	cout<<"rHeight: "<<conParams.rHeight<<endl;
	cout<<"segRev: "<<conParams.segRev<<endl;
}



