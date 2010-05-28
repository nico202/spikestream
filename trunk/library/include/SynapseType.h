#ifndef SYNAPSETYPE_H
#define SYNAPSETYPE_H

//SpikeStream includes
#include "ParameterInfo.h"

//Qt includes
#include <QList>
#include <QString>

namespace spikestream {

	/*! Holds information about a particular synapse type in the SynapseTypes table. */
	class SynapseType {
		public:
			SynapseType(unsigned int synapseTypeID, const QString& description, const QString& paramTableName, const QString& classLibraryName);
			SynapseType(const SynapseType& synType);
			SynapseType& operator=(const SynapseType& rhs);
			~SynapseType();

			QString getClassLibaryName() const;
			QString getDescription() const;
			unsigned int getID() const;
			QList<ParameterInfo> getParameterInfoList() const;
			QString getParameterTableName() const;
			void setParameterInfoList(QList<ParameterInfo>& paramInfoList);


		private:
			//==========================  VARIABLES  =========================
			/*! The id of the neuron type in the SynapseTypes table */
			unsigned int id;

			/*! Description of the neuron type */
			QString description;

			/*! Name of the table in which the parameters are stored for this neuron type */
			QString parameterTableName;

			/*! The parameters available for this synapse type */
			QList<ParameterInfo> parameterInfoList;

			/*! Name of the class library where code for this type is stored.
				NOTE: May not be applicable in all cases. */
			QString classLibraryName;

	};

}

#endif//SYNAPSETYPE_H
