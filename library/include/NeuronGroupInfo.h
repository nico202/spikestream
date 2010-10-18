#ifndef NEURONGROUPINFO_H
#define NEURONGROUPINFO_H

#include <QString>
#include <QHash>

namespace spikestream {

	/*! Holds information about a group of neurons.
		Corresponds to the NeuronGroup table in the SpikeStreamNetwork database */
    class NeuronGroupInfo {
		public:
			NeuronGroupInfo();
			NeuronGroupInfo(unsigned int id, const QString& name, const QString& desc, const QHash<QString, double>& paramMap, unsigned int neurTypeID);
			NeuronGroupInfo(const NeuronGroupInfo& neurGrpInfo);
			NeuronGroupInfo& operator=(const NeuronGroupInfo& rhs);
			~NeuronGroupInfo();

			unsigned int getID() const { return id; }
			QString getName() const { return name; }
			QString getDescription() const { return description; }
			QHash<QString, double> getParameterMap() { return parameterMap; }
			QString getParameterXML();
			unsigned int getNeuronTypeID() const { return neuronTypeID; }
			void setDescription(const QString& description){ this->description = description; }
			void setID(unsigned int id) { this->id = id; }
			void setName(const QString& name){ this->name = name; }

		private:
			//==========================  VARIABLES  ===========================
			/*! Id of the neuron group, which should match that in the database */
			unsigned int id;

			/*! User assigned short name of the group */
			QString name;

			/*! User assigned short description of the group */
			QString description;

			/*! Map of parameters used to create the group */
			QHash<QString, double> parameterMap;

			/*! The ID of the type of neuron in the group. Should correspond to an id
				in the NeuronTypes table in the SpikeStreamNetwork database */
			unsigned int neuronTypeID;


			//===========================  METHODS  ============================
		};

}

#endif//NEURONGROUPINFO_H
