//SpikeStream includes
#include "PluginManager.h"
#include "SpikeStreamException.h"

//Other includes
#include <dlfcn.h>

//Qt includes
#include <QDir>
#include <QtDebug>

/*! Constructor */
PluginManager::PluginManager(QString& pluginFold) throw(SpikeStreamException*){
    //Store widget and plugin folder
    this->pluginFolder = pluginFold;

    //Load plugins from the folder
    loadPlugins();
}

/*! Destructor */
PluginManager::~PluginManager(){

}


/*---------------------------------------------------------------------------------*/
/*----------                  PUBLIC METHODS                          -------------*/
/*---------------------------------------------------------------------------------*/
/*! Returns a list of names of the available plugins */
QStringList PluginManager::getPluginNames() throw(SpikeStreamException*){
    return pluginFunctionMap.keys();
}


/*! Returns plugin as  QWidget */
QWidget* PluginManager::getPlugin(QString pluginName) throw(SpikeStreamException*){
    if(pluginFunctionMap.contains(pluginName)){
	 QWidget* tempWidget = pluginFunctionMap[pluginName]();
	 qDebug()<<"Getting new plugin with name "<<pluginName<<endl;
	 return tempWidget;
    }
    throw new SpikeStreamException("ClassLoader: CANNOT FIND OR RESOLVE CLASS FOR NEURON TYPE: ");
}


/*! Loads up all of the plugins in the plugin directory */
void PluginManager::loadPlugins(){
    //Get a list of files in the plugins directory
    QDir pluginDirectory(this->pluginFolder);
    QStringList filters;
    filters << "*.so";
    pluginDirectory.setNameFilters(filters);
    QStringList fileList = pluginDirectory.entryList();
    qDebug()<<"Available analysis plugins: "<<fileList;

    //Load functions pointing to each plugin
    for(QList<QString>::iterator fileIter = fileList.begin(); fileIter != fileList.end(); ++fileIter){
	QString filePath = pluginDirectory.absolutePath() + "/" + *fileIter;
	qDebug()<<"Loading: "<<filePath;

	//Open the library
	void *hndl = dlopen(filePath.toStdString().data(), RTLD_NOW);
	if(hndl == NULL){
	    QString errorString(dlerror());
	    throw new SpikeStreamException("Cannot open plugin file path. Error: " + errorString);
	}

	//Get functions to create the widget and to get the widget's name
	CreatePluginFunctionType createPluginFunction = (CreatePluginFunctionType) dlsym(hndl, "getClass");
	GetPluginNameFunctionType  getPluginNameFunction = (GetPluginNameFunctionType) dlsym(hndl, "getName");

	//Store the function to create the widget
	if (createPluginFunction  && getPluginNameFunction) {

	    //Get the name of the plugin
	    QString tmpPluginName = getPluginNameFunction();

	    qDebug()<<"Loading plugin with name "<<tmpPluginName;

	    //Store function in map
	    pluginFunctionMap[tmpPluginName] = createPluginFunction;
	}
	else{
	    throw new SpikeStreamException("Failed to obtain required functions from library " + *fileIter);
	}
    }

/*
	Row neurTypeRow(*iter);
    unsigned int neuronType = Utilities::getUShort((std::string) neurTypeRow["TypeID"]);

    //Check for duplicates
    if(neuronFunctionMap.count(neuronType)){
	    SpikeStreamSimulation::systemError("ClassLoader: NEURON TYPE HAS ALREADY BEEN LOADED!", neuronType);
	    return;
    }

    //Get the path to the library
    char* neurLibPath_char = getenv("SPIKESTREAM_ROOT");
    string neuronLibraryPath;
    if(neurLibPath_char != NULL){
	    neuronLibraryPath = neurLibPath_char;
    }
    else{
	    throw spikestreamRootNotFoundException;
    }
    neuronLibraryPath += "/lib/";
    neuronLibraryPath += (std::string)neurTypeRow["ClassLibrary"];
    #ifdef CLASSLOADER_DEBUG
	    cout<<"ClassLoader: Neuron library path = "<<neuronLibraryPath<<endl;
    #endif//CLASSLOADER_DEBUG

    //Try to open the library and get the creating neuron function
    void *hndl = dlopen(neuronLibraryPath.data() , RTLD_NOW);
    if(hndl == NULL){
	    SpikeStreamSimulation::systemError(dlerror());
	    return;
    }
    CreateNeuronFunctionType createNeuronFunction = (CreateNeuronFunctionType) dlsym(hndl, "getClass");
    if ( createNeuronFunction ) {

	    //Output neuron description in debug mode.
	    #ifdef CLASSLOADER_DEBUG
		    Neuron* tempNeuron = createNeuronFunction();
		    cout<<"ClassLoader: Loading neuron of type "<<(*tempNeuron->getDescription())<<endl;
	    #endif//CLASSLOADER_DEBUG

	    //Store function in map
	    neuronFunctionMap[neuronType] = createNeuronFunction;
    }
    else{
	    SpikeStreamSimulation::systemError("ClassLoader: CANNOT FIND OR RESOLVE CLASS WHEN LOADING ALL NEURON CLASSES");
	    return;
    }

    //Store details about parameter table
    neuronParameterTableMap[neuronType] = (std::string) neurTypeRow["ParameterTableName"];
    #ifdef CLASSLOADER_DEBUG
	    cout<<"ClassLoader: Neuron parameter table name is "<<neuronParameterTableMap[neuronType]<<endl;
    #endif//CLASSLOADER_DEBUG
*/

}



