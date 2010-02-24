#ifndef CONFIGEDITOR_H
#define CONFIGEDITOR_H

//Qt includes
#include <QFile>
#include <QHash>
#include <QString>

namespace spikestream {

	/*! Sets parameters in the config file and creates a new config file based on the template if one
		does not exist */
	class ConfigEditor {
		public:
			ConfigEditor();
			~ConfigEditor();

			void setConfigParameters(QHash<QString, QString> newParamMap);

		private:
			//========================  VARIABLES  ========================
			QString rootDirectory;

			//=========================  METHODS  =========================
			void setParameter(QHash<QString, QString>& paramMap, QString& configFileLine);
	};

}

#endif//CONFIGEDITOR_H
