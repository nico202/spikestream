#ifndef TONONINETWORKBUILDER_H
#define TONONINETWORKBUILDER_H

//SpikeStream includes
#include "NetworkDaoThread.h"
#include "NetworksBuilder.h"
using namespace spikestream;

//Qt includes
#include <QString>

namespace spikestream {

    class TononiNetworkBuilder : public NetworksBuilder {
	public:
	    TononiNetworkBuilder();
	    ~TononiNetworkBuilder();
	    void addBalduzziTononiFigure6(const QString& networkName);

	private:
	    //===================  METHODS  ======================

    };

}

#endif//TONONINETWORKBUILDER_H
