#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

//Qt includes
#include <QString>
#include <QHash>

namespace spikestream{

	/*! Loads the configuration from the specified file. */
	class ConfigLoader{
		public:
			ConfigLoader(const QString& fileLocation);
			~ConfigLoader();
			QString getParameter(const QString& paramName);

		private:
			//=========================== VARIABLES =======================================
			/*! Holds the configuration parameters loaded from the file.*/
			QHash<QString, QString> configMap;


			//============================ METHODS ========================================
			/*! Declare copy constructor private so it cannot be used inadvertently.*/
			ConfigLoader(const ConfigLoader&);

			/*! Declare assignment private so it cannot be used inadvertently.*/
			ConfigLoader operator = (const ConfigLoader&);

			void printConfig();
			void setParameter(const QString& configFileLine);
	};

}

#endif//CONFIGLOADER_H
