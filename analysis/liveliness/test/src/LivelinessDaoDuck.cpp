//SpikeStream includes
#include "LivelinessDaoDuck.h"

//Qt includes
#include <QDebug>


/*! Constructor */
LivelinessDaoDuck::LivelinessDaoDuck(){
}


/*! Destructor */
LivelinessDaoDuck::~LivelinessDaoDuck(){
}


/*! Instead of storing cluster in database the complex is stored in a list for testing */
void LivelinessDaoDuck::addCluster(unsigned int, int, QList<unsigned int>& neuronIDList, double liveliness){
	Cluster clstr;
	clstr.setNeuronIDs(neuronIDList);
	clstr.setLiveliness(liveliness);
	clusterList.append(clstr);
}

/*! Resets information stored in the class */
void LivelinessDaoDuck::reset(){
	clusterList.clear();
}

