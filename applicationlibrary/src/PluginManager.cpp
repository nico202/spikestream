//SpikeStream includes
#include "PluginManager.h"
#include "SpikeStreamException.h"
using namespace spikestream;

//Qt includes
#include <QDir>
#include <QtDebug>
#include <QLibrary>


/*! Constructor */
PluginManager::PluginManager(QString& pluginFolder) throw(SpikeStreamException){
	//Store widget and plugin folder
	this->pluginFolder = pluginFolder;

	//Load plugins from the folder
	loadPlugins();
}


/*! Destructor */
PluginManager::~PluginManager(){

}


/*-------------------------------------------------------------------*/
/*--------                  PUBLIC METHODS                    -------*/
/*-------------------------------------------------------------------*/

/*! Returns a list of names of the available plugins */
QStringList PluginManager::getPluginNames() throw(SpikeStreamException){
	return pluginFunctionMap.keys();
}


/*! Returns plugin as  QWidget */
QWidget* PluginManager::getPlugin(QString pluginName) throw(SpikeStreamException){
	if(pluginFunctionMap.contains(pluginName)){
		QWidget* tempWidget = pluginFunctionMap[pluginName]();
		qDebug()<<"Getting new plugin with name "<<pluginName<<endl;
		return tempWidget;
	}
	throw SpikeStreamException("ClassLoader: CANNOT FIND OR RESOLVE CLASS: ");
}


/*-------------------------------------------------------------------*/
/*--------                  PRIVATE METHODS                   -------*/
/*-------------------------------------------------------------------*/

/*! Loads up all of the plugins in the plugin directory */
void PluginManager::loadPlugins(){
	//Get a list of files in the plugins directory
	QDir pluginDirectory(this->pluginFolder);

	//Filter plugin appropriately depending on the operating system
	QStringList filters;
	#ifdef LINUX32_SPIKESTREAM
		filters << "*.so";
	#endif
	#ifdef WIN32_SPIKESTREAM
		filters << "*.dll";
	#endif
	pluginDirectory.setNameFilters(filters);

	//Get list of plugin files
	QStringList fileList = pluginDirectory.entryList();
	qDebug()<<"Path: "<<pluginFolder<<". Available plugins: "<<fileList;

	//Load functions pointing to each plugin
	for(QList<QString>::iterator fileIter = fileList.begin(); fileIter != fileList.end(); ++fileIter){
		QString filePath = pluginDirectory.absolutePath() + "/" + *fileIter;
		qDebug()<<"Loading: "<<filePath;

		//Open the library
		QLibrary tmpLib(filePath);

		//Get functions to create the widget and to get the widget's name
		CreatePluginFunctionType createPluginFunction = (CreatePluginFunctionType) tmpLib.resolve("getClass");
		GetPluginNameFunctionType  getPluginNameFunction = (GetPluginNameFunctionType) tmpLib.resolve("getName");

		//Store the function to create the widget
		if (createPluginFunction  && getPluginNameFunction) {

			//Get the name of the plugin
			QString tmpPluginName = getPluginNameFunction();

			qDebug()<<"Loading plugin with name "<<tmpPluginName;

			//Store function in map
			pluginFunctionMap[tmpPluginName] = createPluginFunction;
		}
		else{
			throw SpikeStreamException("Failed to obtain required functions from library " + *fileIter);
		}
	}
}



