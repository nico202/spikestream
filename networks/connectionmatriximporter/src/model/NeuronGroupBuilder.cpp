//SpikeStream includes
#include "Globals.h"
#include "NetworkDao.h"
#include "NeuronGroupBuilder.h"
#include "SpikeStreamIOException.h"
#include "NeuronType.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <cmath>

//Enable to output debugging information
//#define DEBUG


/*! Constructor */
NeuronGroupBuilder::NeuronGroupBuilder(){
	loadDefaultParameters();
}


/*! Destructor */
NeuronGroupBuilder::~NeuronGroupBuilder(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds neuron groups to the supplied network using the coordinates
	in the file. */
void NeuronGroupBuilder::addNeuronGroups(Network* network, const QString& coordinatesFilePath, const QString& nodeNamesFilePath, QHash<QString, double> parameterMap, urng_t& ranNumGen){
	excitNeurGrpList.clear();
	inhibNeurGrpList.clear();

	//Extract parameters
	storeParameters(parameterMap);

	emit progress(0, 1, "Loading coordinates");
	QList<Point3D> cartCoords = getCartesianCoordinates(coordinatesFilePath);
	float neurGrpDimen = getNeuronGroupDimension(cartCoords);

	loadNodeNames(nodeNamesFilePath);
	if(cartCoords.size() != nodeNameList.size())
		throw SpikeStreamException("Number of node names does not match number of coordinates.");
	emit progress(1, 1, "Coordinates loaded.");

	//Get excitatory and inhibitory neuron types
	NetworkDao networkDao(Globals::getNetworkDao()->getDBInfo());
	NeuronType exNeurType = networkDao.getNeuronType("Izhikevich Excitatory Neuron");
	NeuronType inhibNeurType = networkDao.getNeuronType("Izhikevich Inhibitory Neuron");

	//Check we have default parameters
	if(!defaultParameterMaps.contains(exNeurType.getID()) || !defaultParameterMaps.contains(inhibNeurType.getID()))
		throw SpikeStreamException("Default parameter map(s) missing.");

	//Create neuron groups
	emit progress(0, cartCoords.size()-1, "Adding neuron groups to network.");
	for(int i=0; i<cartCoords.size(); ++i){
		//Create excitatory and inhibitory neuron groups
		NeuronGroup* exNeurGrp = new NeuronGroup(NeuronGroupInfo(0, nodeNameList.at(i),  nodeNameList.at(i), parameterMap, exNeurType));
		NeuronGroup* inhibNeurGrp = new NeuronGroup(NeuronGroupInfo(0, nodeNameList.at(i),  nodeNameList.at(i), parameterMap, inhibNeurType));

		//Set parameters in neuron groups
		exNeurGrp->setParameters(defaultParameterMaps[exNeurGrp->getNeuronTypeID()]);
		inhibNeurGrp->setParameters(defaultParameterMaps[inhibNeurGrp->getNeuronTypeID()]);

		//Add neurons to neuron groups
		addNeurons(exNeurGrp, inhibNeurGrp, neurGrpDimen, cartCoords.at(i), ranNumGen);
		excitNeurGrpList.append(exNeurGrp);
		inhibNeurGrpList.append(inhibNeurGrp);
		emit progress(i, cartCoords.size()-1, "Adding neuron groups to network.");
	}

	//Add neuron groups to the network
	network->addNeuronGroups(excitNeurGrpList);
	network->addNeuronGroups(inhibNeurGrpList);
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Adds the neurons to the neuron groups */
void NeuronGroupBuilder::addNeurons(NeuronGroup* exNeurGrp, NeuronGroup* inhibNeurGrp, float neurGrpDimen, const Point3D& cartCoord, urng_t& ranNumGen){
	//Calculate the number of neurons down each side of the group.
	unsigned numXNeur = (unsigned)floor(cbrt(numberNeuronsPerNode));
	unsigned numYNeur = numXNeur;
	unsigned numZNeur = numberNeuronsPerNode / (numXNeur*numXNeur);

	//Use number of x neurons as the size of the group if neurGrpDimen = 0
	if(neurGrpDimen == 0){
		neurGrpDimen = (float)numXNeur;
	}

	//Calculate the starting position.
	float xStart = cartCoord.getXPos();// - neurGrpDimen/2.0f;
	float yStart = cartCoord.getYPos();// - neurGrpDimen/2.0f;
	float zStart = cartCoord.getZPos();// - neurGrpDimen/2.0f;

	//Calculate the increments on each axis
	float xSpacing, ySpacing, zSpacing;
	if(numXNeur > 1)
		xSpacing = neurGrpDimen / (numXNeur-1);
	else
		xSpacing = neurGrpDimen ;
	if(numYNeur > 1)
		ySpacing = neurGrpDimen / (numYNeur-1);
	else
		ySpacing = neurGrpDimen;
	if(numZNeur > 1)
		zSpacing = neurGrpDimen / (numZNeur-1);
	else
		zSpacing = neurGrpDimen;

	#ifdef DEBUG
		qDebug()<<"Adding neurons. xStart="<<xStart<<"; yStart="<<yStart<<"; zStart="<<zStart<<"; xSpacing="<<xSpacing<<"; ySpacing="<<ySpacing<<"; zSpacing="<<zSpacing;
	#endif//DEBUG

	//Add the neurons to the groups
	float xPos, yPos, zPos;
	for(unsigned xCntr = 0; xCntr < numXNeur; ++xCntr){
		for(unsigned yCntr = 0; yCntr < numYNeur; ++yCntr){
			for(unsigned zCntr = 0; zCntr < numZNeur; ++zCntr){
				xPos = xStart + xCntr*xSpacing;
				yPos = yStart + yCntr*ySpacing;
				zPos = zStart + zCntr*zSpacing;
				if(ranNumGen() < proportionExcitatoryNeurons){
					exNeurGrp->addNeuron(xPos, yPos, zPos);
				}
				else{
					inhibNeurGrp->addNeuron(xPos, yPos, zPos);
				}
			}
		}
	}
}


/*! Extracts a list of points in positive Cartesian coordinates from a file containing a list of
	Talairach coordinates. */
QList<Point3D> NeuronGroupBuilder::getCartesianCoordinates(const QString& coordinatesFile){
	QList<Point3D> talairachCoords = loadTalairachCoordinates(coordinatesFile);

	//Find minimum x, y and z values and max x
	float minX, minY, minZ, maxX;
	bool firstTime = true;
	for(int i=0; i<talairachCoords.size(); ++i){
		Point3D tmpPoint = talairachCoords.at(i);
		if(firstTime){
			minX = tmpPoint.getXPos();
			maxX = minX;
			minY = tmpPoint.getYPos();
			minZ = tmpPoint.getZPos();
			firstTime = false;
		}
		else{
			if(tmpPoint.getXPos() < minX)
				minX = tmpPoint.getXPos();
			if(tmpPoint.getXPos() > maxX)
				maxX = tmpPoint.getXPos();
			if(tmpPoint.getYPos() < minY)
				minY = tmpPoint.getYPos();
			if(tmpPoint.getZPos() < minZ)
				minZ = tmpPoint.getZPos();
		}
	}
	//Only translate if the coordinates are negative
	if(minX < 0.0f)
		minX = Util::toPositive(minX);
	else
		minX = 0.0f;
	if(minY < 0.0f)
		minY = Util::toPositive(minY);
	else
		minY  = 0.0f;
	if(minZ < 0.0f)
		minZ = Util::toPositive(minZ);
	else minZ = 0.0f;

	//Translate max x by same amount
	maxX += minX;

	#ifdef DEBUG
		qDebug()<<"Translating. minX="<<minX<<"; minY="<<minY<<"; minZ="<<minZ;
	#endif//DEBUG

	//Translate and swap around X and Y, so that brain appears horizontally along X axis
	float tmpX;
	for(int i=0; i<talairachCoords.size(); ++i){
		talairachCoords[i].translate(minX, minY, minZ);

		//Flip x so that left and right are preserved after swapping x and y
		talairachCoords[i].setXPos(maxX - talairachCoords[i].getXPos());

		//Reverse x and y
		tmpX = talairachCoords[i].getXPos();
		talairachCoords[i].setXPos(talairachCoords[i].getYPos());
		talairachCoords[i].setYPos(tmpX);

	}
	return talairachCoords;
}


/*! Returns a box that will enable all neuron groups to be placed at the cartesian coordinates
	without overlapping. */
float NeuronGroupBuilder::getNeuronGroupDimension(const QList<Point3D>& cartesianCoordinatesList){
	//Find the minimum spacing between points
	float minDist = 0, tmpDist;
	bool firstTime = true;
	for(int i=0; i<cartesianCoordinatesList.size(); ++i){
		const Point3D& srcPoint = cartesianCoordinatesList.at(i);
		for(int j=0; j<cartesianCoordinatesList.size(); ++j){
			const Point3D& dstPoint = cartesianCoordinatesList.at(j);
			if(srcPoint != dstPoint){
				tmpDist = srcPoint.distance(dstPoint);

				if(firstTime){
					minDist = tmpDist;
					firstTime = false;
				}
				else if(tmpDist < minDist){
					minDist = tmpDist;
				}
			}
		}
	}

	//Multiply minimum distance by 0.8 to give some extra space between groups
	minDist *= nodeSpacingFactor;
	#ifdef DEBUG
		qDebug()<<"Minimum distance between nodes="<<minDist;
	#endif//DEBUG
	return minDist;
}


/*! Loads up the default parameters for the available neuron types. */
void NeuronGroupBuilder::loadDefaultParameters(){
	defaultParameterMaps.clear();
	NetworkDao netDao(Globals::getNetworkDao()->getDBInfo());
	QList<NeuronType> neurTypeList = netDao.getNeuronTypes();
	for(int i=0; i<neurTypeList.size(); ++i)
		defaultParameterMaps[neurTypeList.at(i).getID()] = netDao.getDefaultNeuronParameters(neurTypeList.at(i).getID());
}


/*! Loads up the list of names of nodes */
void NeuronGroupBuilder::loadNodeNames(const QString& nodeNameFileLocation){
	QFile nodeNamesFile(nodeNameFileLocation);
	nodeNameList.clear();
	QHash<QString, bool> nodeNamesMap;//To check for uniqueness

	//Check file exists and open it
	if(!nodeNamesFile.exists())
		throw SpikeStreamIOException("Cannot find node names file.");
	if (!nodeNamesFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open node names file for reading: " + nodeNamesFile.fileName());

	//Load node names from file
	QTextStream in(&nodeNamesFile);
	QString line;
	while (!in.atEnd()) {
		line = in.readLine();
		if(!line.isEmpty()){
			QString nodeName(line.trimmed());
			if(nodeNamesMap.contains(nodeName))
				throw SpikeStreamException("Duplicate node name: " + nodeName);
			nodeNameList.append(nodeName);
			nodeNamesMap[nodeName] = true;
		}
	}
	nodeNamesFile.close();
}


/*! Loads up the file into a list of Talairach coordinates. */
QList<Point3D> NeuronGroupBuilder::loadTalairachCoordinates(const QString& coordinatesFileLocation){
	QFile coordinatesFile(coordinatesFileLocation);
	QList<Point3D> talCoordList;

	//Check file exists and open it
	if(!coordinatesFile.exists())
		throw SpikeStreamIOException("Cannot find Talairach coordinates file.");
	if (!coordinatesFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open coordinates file for reading: " + coordinatesFile.fileName());

	//Load coordinates from file
	QTextStream in(&coordinatesFile);
	QString line;
	while (!in.atEnd()) {
		line = in.readLine();
		if(!line.isEmpty()){
			talCoordList.append(getTalairachCoordinate(line));
		}
	}
	coordinatesFile.close();
	return talCoordList;
}


/*! Extracts a line containing 3 floats and returns it as a point. */
Point3D NeuronGroupBuilder::getTalairachCoordinate(const QString& line){
	QStringList strList = line.split(QRegExp("\\s+"));
	if(strList.size() != 3)
		throw SpikeStreamException("Getting Talairach Coordinates: Incorrect number of dimensions: " + QString::number(strList.size()));
	return Point3D( Util::getFloat(strList.at(0)), Util::getFloat(strList.at(1)), Util::getFloat(strList.at(2)) );
}


/*! Checks and stores the parameters */
void NeuronGroupBuilder::storeParameters(QHash<QString, double>& parameterMap){
	numberNeuronsPerNode = Util::getUIntParameter("neuron_group_size", parameterMap);
	proportionExcitatoryNeurons = Util::getDoubleParameter("proportion_excitatory_neurons", parameterMap);
	nodeSpacingFactor = Util::getFloatParameter("node_spacing_factor", parameterMap);
	if(nodeSpacingFactor == 0.0f)
		throw SpikeStreamException("Group spacing factor cannot be zero.");
}
