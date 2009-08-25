#ifndef NRMDATAIMPORTER_H
#define NRMDATAIMPORTER_H

//Qt includes
#include <QThread>

class NRMDataImporter : public QThread {
    public:
	NRMDataImporter();
	~NRMDataImporter();
	void run();
	void stop();

};

#endif//NRMDATAIMPORTER_H

