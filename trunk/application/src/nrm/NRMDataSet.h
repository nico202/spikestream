#ifndef NRMDATASET_H
#define NRMDATASET_H

//Qt includes
#include <QList>

namespace spikestream {

    class NRMDataSet {
	public:
	    NRMDataSet();
	    ~NRMDataSet();

	    void addData(unsigned char* data);
	    QList<unsigned char*> getDataList() { return dataList; }
	    void reset();

	    void print();
	    int size() { return dataList.size(); }

	    int width;
	    int height;
	    int colorPlanes;

	private:
	    //===========================  VARIABLES  ==========================
	    QList<unsigned char*> dataList;
    };
}

#endif//NRMDATASET_H

