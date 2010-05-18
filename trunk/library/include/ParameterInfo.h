#ifndef PARAMETERINFO_H
#define PARAMETERINFO_H

//Qt includes
#include <QString>

namespace spikestream {

	/*! Holds information about a neuron or synapse parameter.
		This class stores the meta information about a parameter, not the value associated
		with a particular neuron group. */
	class ParameterInfo {
		public:
			ParameterInfo(const QString& name, const QString& description);
			ParameterInfo(const ParameterInfo& info);
			~ParameterInfo();
			QString getDescription() const { return description; }
			QString getName() const { return name; }
			ParameterInfo& operator=(const ParameterInfo& rhs);


		private:
			//====================  VARIABLES  ======================
			/*! The name of the parameter */
			QString name;

			/*! A description of the parameter */
			QString description;

	};

}

#endif//PARAMETERINFO_H
