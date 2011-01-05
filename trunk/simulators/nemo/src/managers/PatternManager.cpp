//SpikeStream includes
#include "PatternManager.h"
#include "SpikeStreamIOException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QFile>
#include <QTextStream>


/*! Constructor */
PatternManager::PatternManager(){
}


/*! Destructor */
PatternManager::~PatternManager(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Static method that loads data from the specified file into the supplied pattern.
	It is the responsibility of the invoking class to delete the pattern
	when it is no longer required. */
void PatternManager::load(const QString &filePath, Pattern& pattern){
	//Reset pattern
	pattern.reset();

	//Open Pattern file
	QFile configFile(filePath);
	if(!configFile.exists())
		throw SpikeStreamIOException("Cannot find config file.");
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open file for reading: " + configFile.fileName());

	//Load contents of file into pattern
	QTextStream in(&configFile);
	QString line;
	while (!in.atEnd()) {
		line = in.readLine();

		//Skip empty lines and comment lines
		if(line.isEmpty() || line.at(0) == '#'){
			;
		}

		//Set the name of the pattern
		else if(line.startsWith("NAME", Qt::CaseInsensitive) ){
			pattern.setName(line.section(':', 1, 1).trimmed());
		}

		//Add a box to the pattern
		else if(line.contains("x", Qt::CaseInsensitive) ) {
			pattern.addBox(getBox(line));
		}
		//Add a point to the pattern
		else if (line.contains(",")){
			pattern.addPoint(getPoint(line));
		}
		else{
			throw SpikeStreamException("Pattern data not recognized: " + line);
		}
	}
	configFile.close();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Extracts a box from the string.
	Box contains points that are less than the second set of coordinates,
	 so add 0.1 to contain all of the points. */
Box PatternManager::getBox(const QString &str){
	//X coordinates
	QString xCoords = str.section('x', 0, 0, QString::SectionSkipEmpty).trimmed();
	float x1 = Util::getFloat(xCoords.section(':', 0, 0, QString::SectionSkipEmpty).trimmed());
	float x2 = 0.1f + Util::getFloat(xCoords.section(':', 1, 1, QString::SectionSkipEmpty).trimmed());

	//Y coordinates
	QString yCoords = str.section('x', 1, 1, QString::SectionSkipEmpty).trimmed();
	float y1 = Util::getFloat(yCoords.section(':', 0, 0, QString::SectionSkipEmpty).trimmed());
	float y2 = 0.1f + Util::getFloat(yCoords.section(':', 1, 1, QString::SectionSkipEmpty).trimmed());

	//Z coordinates
	QString zCoords = str.section('x', 2, 2, QString::SectionSkipEmpty).trimmed();
	float z1 = Util::getFloat(zCoords.section(':', 0, 0, QString::SectionSkipEmpty).trimmed());
	float z2 = 0.1f + Util::getFloat(zCoords.section(':', 1, 1, QString::SectionSkipEmpty).trimmed());

	return Box(x1, y1, z1, x2, y2, z2);
}


/*! Extracts a point from the string */
Point3D PatternManager::getPoint(const QString &str){
	float xPos = Util::getFloat(str.section(',', 0, 0, QString::SectionSkipEmpty).trimmed());
	float yPos = Util::getFloat(str.section(',', 1, 1, QString::SectionSkipEmpty).trimmed());
	float zPos = Util::getFloat(str.section(',', 2, 3, QString::SectionSkipEmpty).trimmed());

	return Point3D(xPos, yPos, zPos);
}
