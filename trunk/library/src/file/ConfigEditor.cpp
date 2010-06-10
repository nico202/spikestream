//SpikeStream includes
#include "ConfigEditor.h"
#include "SpikeStreamException.h"
#include "SpikeStreamIOException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QTextStream>
#include <QCoreApplication>


/*! Constructor */
ConfigEditor::ConfigEditor(){
	//Get root directory.
	rootDirectory = QCoreApplication::applicationDirPath();
	rootDirectory.truncate(rootDirectory.size() - 4);//Trim the "/bin" off the end
}


/*! Destructor */
ConfigEditor::~ConfigEditor(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Sets the parameters in the config file or throws an exception if it cannot be found */
void ConfigEditor::setConfigParameters(QHash<QString, QString> newParamMap){
	/* Make sure that there is a config file already by attempting to copy from the template
	   This function will not overwrite an existing config file */
	QFile::copy (rootDirectory + "/spikestream.config.template", rootDirectory + "/spikestream.config");

	//Create config file
	QFile configFile(rootDirectory + "/spikestream.config");
	if(!configFile.exists())
		throw SpikeStreamIOException("Cannot find config file.");

	//Load the config file into a string
	QString configFileStr;
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SpikeStreamIOException("Cannot open file for reading: " + configFile.fileName());

	QTextStream in(&configFile);
	QString line;
	while (!in.atEnd()) {
		line = in.readLine() + "\n";
		setParameter(newParamMap, line);
		configFileStr += line;
	}
	configFile.close();

	//Write the string back to the file with the modified parameters
	if( !configFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
		throw SpikeStreamIOException("Cannot open file for writing: " + configFile.fileName());

	QTextStream out( &configFile );
	out<<configFileStr;
	configFile.close();

}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Sets the parameter in the line from the config map using the value in the
	parameter map if it exists. */
void ConfigEditor::setParameter(QHash<QString, QString>& paramMap, QString& configFileLine){
	//Do nothing if line is a comment or is empty
	if(configFileLine.isEmpty() || configFileLine.at(0) == '#')
		return;

	//Get the parameter for this line
	QString paramName = configFileLine.section("=", 0, 0, QString::SectionSkipEmpty).trimmed();
	for(QHash<QString, QString>::iterator iter = paramMap.begin(); iter != paramMap.end(); ++iter){
		if(iter.key() == paramName){
			configFileLine = paramName + " = " + iter.value() + "\n";
			return;
		}
	}
}


