//SpikeStream includes
#include "NRMDataSetImporter.h"
#include "NRMException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <sstream>
using namespace std;


/*! Construtor */
NRMDataSetImporter::NRMDataSetImporter(){
}


/*! Destructor */
NRMDataSetImporter::~NRMDataSetImporter(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Loads a dataset from the specified file */
void NRMDataSetImporter::loadDataSet(const char* filePath){
    FILE* file = fopen(filePath, "rb");
    if ( !file )
		throw NRMException("Unable to open data set file.");

    //Reset count of bytes we have read
    fileByteCount = 0;

    //Read in the version of the dataset
    int datSetVer;
    char txt[10];

    fReadFile(txt, 9, 1, file);
    txt[9] = '\0';

    if(QString(txt) != "MacConD02")
		throw NRMException("Dataset version not recognized.");

    //Dataset version should be 2 if we have reached this point
    datSetVer = 2;

    //Reset dataset
    dataSet.reset();

    //Initialize variables used in loading
    unsigned short val;
    int val2;
    int numImgs;

    //Get the width, height and color planes of the dataset
    fReadFile(&val, 2, 1, file);
    dataSet.width = val;
    fReadFile(&val, 2, 1, file);
    dataSet.height = val;
    fReadFile(&val, 2, 1, file);
    dataSet.colorPlanes = val;
    if ( datSetVer > 1 )  {
		fReadFile(&val2, 4, 1, file);
		numImgs = val2;
    }
    else {
		fReadFile(&val, 2, 1, file);
		numImgs = val;
    }

    //Read in the data from the file
    for ( int n = 0; n < numImgs; n++ ) {
		//Create array to hold data
		unsigned char* tmpArray = new unsigned char[dataSet.width * dataSet.height];

		//Read data into array
		fReadFile(tmpArray, dataSet.height * dataSet.width, 1, file);

		//Add array to dataset
		dataSet.addData(tmpArray);
    }

    if ( ferror(file) )
		throw NRMException("Error in data set file.");

    //Close the file
    fclose(file);

    //dataSet.print();
}


/*! Resets class ready for another load */
void NRMDataSetImporter::reset(){
    fileByteCount = 0;
    dataSet.reset();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Wrapper for fread that does error checking and throws an exception if an unexpected number of bytes is read */
void NRMDataSetImporter::fReadFile(void* dataStruct, size_t sizeOfElement, size_t numElements, FILE* file ){
    size_t result = fread(dataStruct, sizeOfElement, numElements, file);
    if(result != numElements){
		cout<<"Actual number of elements read="<<result<<"; expected number of elements = "<<numElements<<endl;
		if( feof(file) ){
			ostringstream errMsg;
			errMsg<<"Unexpected end of file. Tried to read "<<sizeOfElement * numElements<<" elements. ";
			errMsg<<"Read "<<result<<" elements. ";
			errMsg<<"ByteCount="<<fileByteCount;
			throw NRMException (errMsg.str());
		}
		else if( ferror(file) ){
			throw NRMException ("ferror encountered reading from file", fileByteCount);
		}
		else{
			throw NRMException ("Unknown error encountered when reading from file. ByteCount=", fileByteCount);
		}
    }
    else{
		fileByteCount += (sizeOfElement * numElements);
    }
}
