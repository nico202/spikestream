#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

//SpikeStream includes
#include "SpikeStreamException.h"

//Qt includes
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QHash>

/*! The type of the function used to create plugins. */
typedef QWidget* (*CreatePluginFunctionType)();

/*! The type of the function used to create neurons. */
typedef QString (*GetPluginNameFunctionType)();

namespace spikestream {

	/*! Loads up plugins from the specified folder and throws exception if there is a problem. */
	class PluginManager {
		public:
			PluginManager(QString& pluginFolder) throw(SpikeStreamException);
			~PluginManager();
			QStringList getPluginNames() throw(SpikeStreamException);
			QWidget* getPlugin(QString pluginName) throw(SpikeStreamException);
			void loadPlugins();


		private:
			//============================== VARIABLES ============================
			/*! Location where plugins are stored. */
			QString pluginFolder;

			/*! Map holding function pointers that create neurons of each type.*/
			QHash<QString, CreatePluginFunctionType> pluginFunctionMap;


			//=============================== METHODS =============================
    };

}

#endif // PLUGINMANAGER_H
