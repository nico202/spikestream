//SpikeStream includes
#include "Globals.h"
#include "NetworkDao.h"
#include "TruthTableModel.h"
using namespace spikestream;

//Qt includes
#include <QDebug>

//Other includes
#include <iostream>
using namespace std;

/*! Constructor */
TruthTableModel::TruthTableModel() : QAbstractTableModel(){
}


/*! Destructor */
TruthTableModel::~TruthTableModel(){
}


/*-------------------------------------------------------------*/
/*-------                  PUBLIC METHODS                ------*/
/*-------------------------------------------------------------*/

/*! Inherited from QAbstractTableModel. Returns the number of columns in the model */
int TruthTableModel::columnCount(const QModelIndex&) const{
    return headerList.size() + 1;
}


/*! Inherited from QAbstractTableModel. Returns the data at the specified model index
    ready to be displayed in the way requested */
QVariant TruthTableModel::data(const QModelIndex & index, int role) const{
    //Return invalid index if index is invalid or no network loaded
    if (!index.isValid())
	return QVariant();
    if(!Globals::networkLoaded())
	return QVariant();

    //Check rows and columns are in range
    if (index.row() < 0 || index.row() >= rowCount() || index.column() < 0 || index.column() >= columnCount())
	return QVariant();

    //Return appropriate data
    if (role == Qt::DisplayRole){
	cout<<"DATA: ("<<index.row()<<","<<index.column()<<") value: "<<dataList.at(index.row()).at(index.column())<<endl;
	return dataList.at(index.row()).at(index.column());
    }


    //If we have reached this point ignore request
    return QVariant();
}


/*! Inherited from QAbstractTableModel. Returns the header data */
QVariant TruthTableModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole)
	return QVariant();

    if (orientation == Qt::Horizontal){
	if(section < headerList.size())
	    return headerList.at(section);

	if(section == headerList.size())
	    return "Output";
    }

    return QVariant();

}


/*! Inherited from QAbstractTableModel. Returns the number of rows in the model. */
int TruthTableModel::rowCount(const QModelIndex&) const{
    return dataList.size();
}


void TruthTableModel::setNeuronID(unsigned int neuronID){
    //Reset the class
    clearModelData();

    WeightlessNeuron* neuron = Globals::getNetworkDao()->getWeightlessNeuron(neuronID);

    //Build empty header list
    for(int i=0; i<neuron->getNumberOfConnections(); ++i)
	headerList.append(0);

    /* Store the headers
       Each position in the header list corresponds to a particular neuron id */
    QHash<unsigned int, QList<unsigned int> > connectionMap = neuron->getConnectionMap();
    for(QHash<unsigned int, QList<unsigned int> >::iterator mapIter = connectionMap.begin(); mapIter != connectionMap.end(); ++mapIter){
	//Work through connections for the neuron
	foreach(unsigned int index, mapIter.value()){
	    headerList[index] = mapIter.key();
	}
    }

    //Get training data
    QList<unsigned char*> trainingDataList = neuron->getTrainingData();

    //Add training data to list
    for(int i=0; i<trainingDataList.size(); ++i){//Work through the arrays of training data
	QList<unsigned int> tmpList;
	for(int j=0; j<headerList.size() + 1; ++j){
	    tmpList.append(trainingDataList.at(i)[j/8 + 1] & 1<<j%8);
	}

	//Add output
	tmpList.append((unsigned int)trainingDataList[0]);

	//Store list
	dataList.append(tmpList);
    }
    cout<<"DATA LIST SIZE: "<<dataList.size()<<" header siaze: "<<headerList.size() + 1<<endl;

    //Clean up
    delete neuron;

}


/*! Clears the data stored in the class */
void TruthTableModel::clearModelData(){
  dataList.clear();
  headerList.clear();
}


