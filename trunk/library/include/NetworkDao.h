#ifndef NETWORKDAO_H
#define NETWORKDAO_H

//SpikeStream includes
#include "SpikeStreamVolume.h"
#include "AbstractDao.h"

//Qt includes
#include <QString>


/*! Abstraction layer on top of the SpikeStreamNetwork database.*/
class NetworkDao : public AbstractDao {
    public:
	NetworkDao(DBInfo& dbInfo);
	virtual ~NetworkDao();

	QList<ConnectionGroupInfo*> getConnectionGroupsInfo();
	void getConnections(NeuronGroup* connGrp);
	QList<NeuronGroupInfo*> getNeuronGroupsInfo();
	void getNeurons(NeuronGroup* neurGrp);

    private:


};

#endif//NETWORKDAO_H


