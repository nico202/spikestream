#ifndef NRMDATASETIMPORTER_H
#define NRMDATASETIMPORTER_H

//SpikeStream includes
#include "NRMDataSet.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;

namespace spikestream {

    /*! Loads a data set from NRM *.set files */
    class NRMDataSetImporter {
	public:
	    NRMDataSetImporter();
	    ~NRMDataSetImporter();

	    NRMDataSet* getDataSet() { return &dataSet; }
	    void loadDataSet(const char* filePath);
	    void reset();

	private:
	    /*! Record of the number of bytes read from file for debugging */
	    int fileByteCount;

	    /*! Dataset holding loaded data */
	    NRMDataSet dataSet;


	    void fReadFile(void* dataStruct, size_t sizeOfElement, size_t numElements, FILE* file );
    };
}

#endif//NRMDATASETIMPORTER_H



