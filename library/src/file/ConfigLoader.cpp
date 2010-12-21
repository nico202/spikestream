//SpikeStream includes
#include "ConfigLoader.h"
#include "SpikeStreamIOException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor loads up the configuration data from the given file path. */
ConfigLoader::ConfigLoader(){
	QString rootDirectory = Util::getRootDirectory();

	/* Make sure that there is a config file already by attempting to copy from the template
	   This function will not overwrite an existing config file */
	QFile::copy (rootDirectory + "/spikestream.config.template", rootDirectory + "/spikestream.config");

	//Create config file
	QFile configFile(rootDirectory + "/spikestream.config");
	if(!configFile.exists()){
		QDir tmpDir(rootDirectory);
		throw SpikeStreamIOException("Cannot find config file in directory: " + tmpDir.absolutePath());
	}

	//Load the config file into the map
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open file for reading: " + configFile.fileName());

	QTextStream in(&configFile);
	QString line;
	while (!in.atEnd()) {
		line = in.readLine();
		setParameter(line);
	}
	configFile.close();
}


/*! Destructor. */
ConfigLoader::~ConfigLoader(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Extracts the configuration parameter as a QString. */
QString ConfigLoader::getParameter(const QString& paramName){
	if(!configMap.contains(paramName))
		throw SpikeStreamException("Configuration file does not contain a parameter with name: " + paramName);
	return configMap[paramName];
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Debug method for printing out the loaded configuration parameters. */
void ConfigLoader::printConfig(){
	cout<<"-----------  SpikeStream Configuration  -----------"<<endl;
	for (QHash<QString, QString>::iterator iter = configMap.begin(); iter != configMap.end(); ++iter) {
		cout << "Key: " << iter.key().toStdString() << "; "<< "Value: " << iter.value().toStdString()<< endl;
	}
}


/*! Sets the parameter in the line from the config map using the value in the
	parameter map if it exists. */
void ConfigLoader::setParameter(const QString& configFileLine){
	//Do nothing if line is a comment or is empty
	if(configFileLine.isEmpty() || configFileLine.at(0) == '#')
		return;

	//Extract the parameter for this line
	QString paramName = configFileLine.section("=", 0, 0, QString::SectionSkipEmpty).trimmed();
	QString paramValue = configFileLine.section("=", 1, 1, QString::SectionSkipEmpty).trimmed();

	if(configMap.contains(paramName))
		throw SpikeStreamException("Duplicate parameter in config file: " + paramName);

	configMap[paramName] = paramValue;
}


