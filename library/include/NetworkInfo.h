#ifndef NETWORKINFO_H
#define NETWORKINFO_H

//Qt includes
#include <QString>

namespace spikestream{

    class NetworkInfo {
	public:
	    NetworkInfo();
	    NetworkInfo(unsigned int id, const QString& name, const QString& desc, bool locked);
	    NetworkInfo(const NetworkInfo& netInfo);
	    ~NetworkInfo();
	    NetworkInfo& operator=(const NetworkInfo& rhs);

	    unsigned int getID(){ return id; }
	    QString getDescription(){ return description; }
	    QString getName(){ return name; }
	    bool isLocked(){ return locked; }
	    void setID(unsigned int id) { this->id = id; }
	    void setLocked(bool locked) { this->locked = locked; }
	    void setName(const QString& name) { this->name = name; }
	    void setDescription(const QString& description) { this->description = description; }

	private:
	    /*! Id of the network in SpikeStreamNetwork database */
	    unsigned int id;

	    /*! Name of the network */
	    QString name;

	    /*! Description of the network */
	    QString description;

	    /*! Whether the network is editable or not.
		Networks associated with archived simulation runs are not editable */
	    bool locked;

    };
}

#endif//NETWORKINFO_H

