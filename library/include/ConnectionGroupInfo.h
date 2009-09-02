#ifndef CONNECTIONGROUPINFO_H
#define CONNECTIONGROUPINFO_H


namespace SpikeStream {

    /*! Holds information about a connection group as stored in the ConnectionGroup table
	    of the SpikeStreamNetwork database */
    class ConnectionGroupInfo {


	unsigned int fromLayerID;
	unsigned int toLayerID;
	unsigned short connectionType;
	unsigned short minDelay;
	unsigned short maxDelay;
	unsigned int connectionGrpID;
	unsigned int synapseType;
	map<string, double> paramMap;


    };

}

#endif//CONNECTIONGROUPINFO_H


