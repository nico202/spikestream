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
			//====================  METHODS  ========================
			ParameterInfo(const QString& name, const QString& description, int type);
			ParameterInfo(const ParameterInfo& info);
			~ParameterInfo();
			QString getDescription() const { return description; }
			QString getName() const { return name; }
			int getType() const { return type; }
			ParameterInfo& operator=(const ParameterInfo& rhs);


			//=================  STATIC VARIABLES  ==================
			/*! Parameter is a double */
			static const int DOUBLE = 1;

			/*! Parameter is a boolean */
			static const int BOOLEAN = 2;

			/*! Parameter is an integer */
			static const int INTEGER = 3;

			/*! Parameter is an unsigned integer */
			static const int UNSIGNED_INTEGER = 4;


		private:
			//====================  VARIABLES  ======================
			/*! The name of the parameter */
			QString name;

			/*! A description of the parameter */
			QString description;

			/*! The type of the parameter - double, boolean, etc. */
			int type;

	};

}

#endif//PARAMETERINFO_H
