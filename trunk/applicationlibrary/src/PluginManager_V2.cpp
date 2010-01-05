//SpikeStream includes
#include "PluginManager_V2.h"
#include "SpikeStreamException.h"

//Other includes
//#include <dlfcn.h>

//Qt includes
#include <QDir>
#include <QtDebug>
#include <QLibrary>

/*! Constructor */
PluginManager_V2::PluginManager_V2(QString& pluginFold) throw(SpikeStreamException){
    //Store widget and plugin folder
    this->pluginFolder = pluginFold;

    //Load plugins from the folder
    loadPlugins();
}

/*! Destructor */
PluginManager_V2::~PluginManager_V2(){

}


/*---------------------------------------------------------------------------------*/
/*----------                  PUBLIC METHODS                          -------------*/
/*---------------------------------------------------------------------------------*/
/*! Returns a list of names of the available plugins */
QStringList PluginManager_V2::getPluginNames() throw(SpikeStreamException){
    return pluginFunctionMap.keys();
}


/*! Returns plugin as  QWidget */
QWidget* PluginManager_V2::getPlugin(QString pluginName) throw(SpikeStreamException){
    if(pluginFunctionMap.contains(pluginName)){
		QWidget* tempWidget = pluginFunctionMap[pluginName]();
		qDebug()<<"Getting new plugin with name "<<pluginName<<endl;
		return tempWidget;
    }
    throw SpikeStreamException("ClassLoader: CANNOT FIND OR RESOLVE CLASS: ");
}


/*! Loads up all of the plugins in the plugin directory */
void PluginManager_V2::loadPlugins(){
    //Get a list of files in the plugins directory
    QDir pluginDirectory(this->pluginFolder);
    QStringList filters;
    filters << "*.so";
    pluginDirectory.setNameFilters(filters);
    QStringList fileList = pluginDirectory.entryList();
    qDebug()<<"Path: "<<pluginFolder<<". Available plugins: "<<fileList;

    //Load functions pointing to each plugin
    for(QList<QString>::iterator fileIter = fileList.begin(); fileIter != fileList.end(); ++fileIter){
		QString filePath = pluginDirectory.absolutePath() + "/" + *fileIter;
		qDebug()<<"Loading: "<<filePath;

		//Open the library
		QLibrary tmpLib(filePath);
		if(!tmpLib.isLoaded())
			throw SpikeStreamException("Library " + filePath + " could not be loaded.");

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



