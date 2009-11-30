#ifndef TONONINETWORKBUILDER_H
#define TONONINETWORKBUILDER_H

//SpikeStream includes
#include "NetworkDaoThread.h"
using namespace spikestream;

//Qt includes
#include <QString>

namespace spikestream {

    class TononiNetworkBuilder {
	public:
	    TononiNetworkBuilder();
	    ~TononiNetworkBuilder();
	    void addBalduzziTononiFigure6(const QString& networkName);

	private:
	    //===================  METHODS  ======================
	    void runThread(NetworkDaoThread& thread);
    };

}

#endif//TONONINETWORKBUILDER_H
