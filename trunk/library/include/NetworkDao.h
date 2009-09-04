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

    /*! Abstraction layer on top of the SpikeStreamNetwork database.*/
    class NetworkDao : public AbstractDao {
	public:
	    NetworkDao(DBInfo& dbInfo);
	    virtual ~NetworkDao();

	    QList<ConnectionGroupInfo> getConnectionGroupsInfo(unsigned int networkID);
	    void getConnections(ConnectionGroup* connGrp);
	    QList<NetworkInfo> getNetworksInfo();
	    QList<NeuronGroupInfo> getNeuronGroupsInfo(unsigned int networkID);
	    void getNeurons(NeuronGroup* neurGrp);

	private:
	    //==========================  VARIABLES  ==========================


    };

}

#endif//NETWORKDAO_H


