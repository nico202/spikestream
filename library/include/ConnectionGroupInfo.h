#ifndef CONNECTIONGROUPINFO_H
#define CONNECTIONGROUPINFO_H

//Qt includes
#include <QString>
#include <QHash>

namespace spikestream {

    /*! Holds information about a connection group as stored in the ConnectionGroup table
	    of the SpikeStreamNetwork database */
    class ConnectionGroupInfo {
	public:
	    ConnectionGroupInfo();
	    ConnectionGroupInfo(unsigned int id, const QString& desc, unsigned int fromID, unsigned int toID, QHash<QString, double> paramMap, unsigned int synType);
	    ConnectionGroupInfo(const ConnectionGroupInfo& conGrpInfo);
	    ~ConnectionGroupInfo();
	    ConnectionGroupInfo& operator=(const ConnectionGroupInfo& rhs);

	    unsigned int getID() const { return id; }
	    QString getDescription() const { return description; }
	    unsigned int getFromNeuronGroupID() const { return fromNeuronGroupID; }
	    unsigned int getToNeuronGroupID() const { return toNeuronGroupID; }
	    unsigned int getSynapseType() const { return synapseType; }
	    QHash<QString, double> getParameterMap() { return parameterMap; }
	    QString getParameterXML();

	    void setID(unsigned int id) { this->id = id; }

	private:
	    /*! ID of the connection group. Should match a row in the ConnectionGroup
		table in the SpikeStream database */
	    unsigned int id;

	    /*! Description of the connection */
	    QString description;

	    /*! ID of the neuron group that the connection is from */
	    unsigned int fromNeuronGroupID;

	    /*! ID of the neuron group that the connection is to */
	    unsigned int toNeuronGroupID;

	    /*! Type of synapses on the connection */
	    unsigned int synapseType;

	    /*! Parameters of the connection */
	    QHash<QString, double> parameterMap;

    };

}

#endif//CONNECTIONGROUPINFO_H


