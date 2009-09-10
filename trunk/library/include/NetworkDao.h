#ifndef NETWORKDAO_H
#define NETWORKDAO_H

//SpikeStream includes
#include "AbstractDao.h"
#include "ConnectionGroup.h"
#include "ConnectionGroupInfo.h"
#include "NetworkInfo.h"
#include "NeuronGroup.h"
#include "NeuronGroupInfo.h"
using namespace spikestream;

//Qt includes
#include <QString>

namespace spikestream{

    /*! Abstraction layer on top of the SpikeStreamNetwork database.
	Used for light operations taking place in the same thread as the calling
	function. Operations that are more database intensive can be found in
	the NetworkDaoThread class. */
    class NetworkDao : public AbstractDao {
	public:
	    NetworkDao(DBInfo& dbInfo);
	    virtual ~NetworkDao();

	    void addNetwork(NetworkInfo& netInfo);

	    QList<NetworkInfo> getNetworksInfo();
	    QList<NeuronGroupInfo> getNeuronGroupsInfo(unsigned int networkID);
	    QList<ConnectionGroupInfo> getConnectionGroupsInfo(unsigned int networkID);


	private:
	    //==========================  VARIABLES  ==========================


    };

}

#endif//NETWORKDAO_H


