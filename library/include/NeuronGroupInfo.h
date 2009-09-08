#ifndef NEURONGROUPINFO_H
#define NEURONGROUPINFO_H

#include <QString>
#include <QHash>

namespace spikestream {

    class NeuronGroupInfo {
	public:
	    NeuronGroupInfo();
	    NeuronGroupInfo(unsigned int id, const QString& name, const QString& desc, const QHash<QString, double>& paramMap, int neurType, int numNeur);
	    NeuronGroupInfo(const NeuronGroupInfo& neurGrpInfo);
	    NeuronGroupInfo& operator=(const NeuronGroupInfo& rhs);
	    ~NeuronGroupInfo();

	    unsigned int getID() { return id; }
	    QString getName() { return name; }
	    QString getDescription() { return description; }
	    QHash<QString, double> getParameterMap() { return parameterMap; }
	    QString getParameterXML();
	    unsigned int getNeuronType() { return neuronType; }
	    unsigned int getNumberOfNeurons() { return numberOfNeurons; }

	    void setID(unsigned int id) { this->id = id; }

	private:
	    /*! Id of the neuron group, which should match that in the database */
	    unsigned int id;

	    /*! User assigned short name of the group */
	    QString name;

	    /*! User assigned short description of the group */
	    QString description;

	    /*! Map of parameters used to create the group */
	    QHash<QString, double> parameterMap;

	    /*! The type of neuron in the group. Should correspond to an id
		in the NeuronTypes table in the SpikeStreamNetwork database */
	    int neuronType;

	    /*! Number of neurons in the group
		NOTE: Take care that this value matches the loaded size of the network. */
	    int numberOfNeurons;

    };

}

#endif//NEURONGROUPINFO_H
