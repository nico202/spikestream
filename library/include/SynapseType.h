#ifndef SYNAPSETYPE_H
#define SYNAPSETYPE_H

//Qt includes
#include <QString>

namespace spikestream {

	/*! Holds information about a particular neuron type in the SynapseTypes table. */
	class SynapseType {
		public:
			SynapseType(unsigned int synapseTypeID, const QString& description, const QString& paramTableName, const QString& classLibraryName);
			SynapseType(const SynapseType& synType);
			SynapseType& operator=(const SynapseType& rhs);
			~SynapseType();

			unsigned int getID() const;
			QString getDescription() const;
			QString getParameterTableName() const;
			QString getClassLibaryName() const;

		private:
			//==========================  VARIABLES  =========================
			/*! The id of the neuron type in the SynapseTypes table */
			unsigned int id;

			/*! Description of the neuron type */
			QString description;

			/*! Name of the table in which the parameters are stored for this neuron type */
			QString parameterTableName;

			/*! Name of the class library where code for this type is stored.
				NOTE: May not be applicable in all cases. */
			QString classLibraryName;

	};

}

#endif//SYNAPSETYPE_H
