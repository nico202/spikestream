#ifndef NEURONTYPE_H
#define NEURONTYPE_H

//SpikeStream includes
#include "ParameterInfo.h"

//Qt includes
#include <QList>
#include <QString>

namespace spikestream {

	/*! Holds information about a particular neuron type in the NeuronTypes table. */
	class NeuronType {
		public:
			NeuronType(unsigned int neuronTypeID, const QString& description, const QString& paramTableName, const QString& classLibraryName);
			NeuronType(const NeuronType& neurType);
			NeuronType& operator=(const NeuronType& rhs);
			~NeuronType();

			unsigned int getID() const;
			QString getDescription() const;
			QString getParameterTableName() const;
			QList<ParameterInfo> getParameterInfoList() const;
			QString getClassLibaryName() const;
			void setParameterInfoList(QList<ParameterInfo>& paramInfoList);

		private:
			//==========================  VARIABLES  =========================
			/*! The id of the neuron type in the NeuronTypes table */
			unsigned int id;

			/*! Description of the neuron type */
			QString description;

			/*! Name of the table in which the parameters are stored for this neuron type */
			QString parameterTableName;

			/*! The parameters available for this neuron type */
			QList<ParameterInfo> parameterInfoList;

			/*! Name of the class library where code for this type is stored.
				NOTE: May not be applicable in all cases. */
			QString classLibraryName;

	};

}

#endif//NEURONTYPE_H
