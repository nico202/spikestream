#ifndef NETWORKINFO_H
#define NETWORKINFO_H

//Qt includes
#include <QString>

namespace spikestream{

	/*! Holds information about a particular network.
		Corresponds to the Network table in the SpikeStreamNetwork database. */
    class NetworkInfo {
		public:
			NetworkInfo();
			NetworkInfo(unsigned int id, const QString& name, const QString& desc);
			NetworkInfo(const NetworkInfo& netInfo);
			~NetworkInfo();
			NetworkInfo& operator=(const NetworkInfo& rhs);

			unsigned int getID(){ return id; }
			QString getDescription(){ return description; }
			QString getName(){ return name; }
			void setID(unsigned int id) { this->id = id; }
			void setName(const QString& name) { this->name = name; }
			void setDescription(const QString& description) { this->description = description; }

		private:
			/*! Id of the network in SpikeStreamNetwork database */
			unsigned int id;

			/*! Name of the network */
			QString name;

			/*! Description of the network */
			QString description;

		};
}

#endif//NETWORKINFO_H

