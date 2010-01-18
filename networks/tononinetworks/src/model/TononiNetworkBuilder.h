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
	    void addBalduzziTononiFigure5(const QString& networkName, const QString& networkDescription);
	    void addBalduzziTononiFigure6(const QString& networkName, const QString& networkDescription);
	    void addBalduzziTononiFigure12(const QString& networkName, const QString& networkDescription);

	private:
	    //===================  METHODS  ======================
	    void addParityGateTraining(unsigned int neuronID, int numberOfConnections);
    };

}

#endif//TONONINETWORKBUILDER_H
