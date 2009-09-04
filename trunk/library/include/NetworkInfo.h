#ifndef NETWORKINFO_H
#define NETWORKINFO_H

//Qt includes
#include <QString>

namespace spikestream{

    class NetworkInfo {
	public:
	    NetworkInfo();
	    NetworkInfo(unsigned int id, const QString& name, bool locked);
	    NetworkInfo(const NetworkInfo& netInfo);
	    ~NetworkInfo();
	    NetworkInfo& operator=(const NetworkInfo& rhs);

	    unsigned int getID(){ return id; }
	    QString getName(){ return name; }
	    bool isLocked(){ return locked; }

	private:
	    /*! Id of the network in SpikeStreamNetwork database */
	    unsigned int id;

	    /*! Name of the network */
	    QString name;

	    /*! Whether the network is editable or not.
		Networks associated with archived simulation runs are not editable */
	    bool locked;

    };
}

#endif//NETWORKINFO_H

