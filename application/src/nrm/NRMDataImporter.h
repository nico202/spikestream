#ifndef NRMDATAIMPORTER_H
#define NRMDATAIMPORTER_H

//SpikeStream includes
#include "DBInfo.h"
using namespace spikestream;

//Qt includes
#include <QThread>

class NRMDataImporter : public QThread {
    public:
	NRMDataImporter(const DBInfo& dbInfo);
	~NRMDataImporter();
	void run();
	void stop();

};

#endif//NRMDATAIMPORTER_H

