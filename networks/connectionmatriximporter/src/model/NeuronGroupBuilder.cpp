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
void NeuronGroupBuilder::addNeuronGroups(Network* network, const QString& coordinatesFilePath, const QString& nodeNamesFilePath, QHash<QString, double> parameterMap){
	excitNeurGrpList.clear();
	inhibNeurGrpList.clear();

	//Extract parameters
	unsigned numNeurPerGroup = Util::getUIntParameter("neuron_group_size", parameterMap);
	double proportionExcitatoryNeur = Util::getDoubleParameter("proportion_excitatory_neurons", parameterMap);

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
		addNeurons(exNeurGrp, inhibNeurGrp, numNeurPerGroup, proportionExcitatoryNeur, neurGrpDimen, cartCoords.at(i));
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
void NeuronGroupBuilder::addNeurons(NeuronGroup* exNeurGrp, NeuronGroup* inhibNeurGrp, unsigned numNeurPerGroup, double proportionExcitatoryNeur, float neurGrpDimen, const Point3D& cartCoord){
	int inhibThreshold = Util::rUInt(proportionExcitatoryNeur * RAND_MAX);

	//Calculate the number of neurons down each side of the group.
	unsigned numXNeur = (unsigned)floor(cbrt(numNeurPerGroup));
	unsigned numYNeur = numXNeur;
	unsigned numZNeur = numNeurPerGroup / (numXNeur*numXNeur);

	//Calculate the starting position.
	float xStart = cartCoord.getXPos() - neurGrpDimen/2.0f;
	float yStart = cartCoord.getYPos() - neurGrpDimen/2.0f;
	float zStart = cartCoord.getZPos() - neurGrpDimen/2.0f;

	//Calculate the increments on each axis
	float xSpacing = neurGrpDimen / numXNeur-1;
	float ySpacing = neurGrpDimen / numYNeur-1;
	float zSpacing = neurGrpDimen / numZNeur-1;

	//Add the neurons to the groups
	float xPos, yPos, zPos;
	for(unsigned xCntr = 0; xCntr < numXNeur; ++xCntr){
		for(unsigned yCntr = 0; yCntr < numYNeur; ++yCntr){
			for(unsigned zCntr = 0; zCntr < numZNeur; ++zCntr){
				xPos = xStart + xCntr*xSpacing;
				yPos = yStart + yCntr*ySpacing;
				zPos = zStart + zCntr*zSpacing;
				if(rand() <= inhibThreshold){
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

	//Find minimum x, y and z values
	float minX, minY, minZ;
	bool firstTime = true;
	for(int i=0; i<talairachCoords.size(); ++i){
		Point3D tmpPoint = talairachCoords.at(i);
		if(firstTime){
			minX = tmpPoint.getXPos();
			minY = tmpPoint.getYPos();
			minZ = tmpPoint.getZPos();
			firstTime = false;
		}
		else{
			if(tmpPoint.getXPos() < minX)
				minX = tmpPoint.getXPos();
			if(tmpPoint.getYPos() < minY)
				minY = tmpPoint.getYPos();
			if(tmpPoint.getZPos() < minZ)
				minZ = tmpPoint.getZPos();
		}
	}

	minX = Util::toPositive(minX);
	minY = Util::toPositive(minY);
	minZ = Util::toPositive(minZ);

	for(int i=0; i<talairachCoords.size(); ++i){
		talairachCoords[i].translate(minX, minY, minZ);
	}
	return talairachCoords;
}


/*! Returns a box that will enable all neuron groups to be placed at the cartesian coordinates
	without overlapping. */
float NeuronGroupBuilder::getNeuronGroupDimension(const QList<Point3D>& cartesianCoordinatesList){
	//Find the minimum spacing between points
	float minDist = 0, xDist, yDist, zDist;
	bool firstTime = true;
	for(int i=0; i<cartesianCoordinatesList.size(); ++i){
		const Point3D& srcPoint = cartesianCoordinatesList.at(i);
		for(int j=0; j<cartesianCoordinatesList.size(); ++j){
			const Point3D& dstPoint = cartesianCoordinatesList.at(j);
			xDist = Util::toPositive(srcPoint.getXPos() - dstPoint.getXPos());
			yDist = Util::toPositive(srcPoint.getYPos() - dstPoint.getYPos());
			zDist = Util::toPositive(srcPoint.getZPos() - dstPoint.getZPos());

			if(firstTime){
				minDist = xDist;
				if(yDist < minDist)
					minDist = yDist;
				if(zDist < minDist)
					minDist = zDist;
				firstTime = false;
			}
			else{
				if(xDist < minDist)
					minDist = xDist;
				if(yDist < minDist)
					minDist = yDist;
				if(zDist < minDist)
					minDist = zDist;
			}
		}
	}

	//Multiply minimum distance by 0.8 to give some extra space between groups
	minDist *= 0.8;

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
