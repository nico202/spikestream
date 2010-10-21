#ifndef PATTERN_H
#define PATTERN_H

//Qt includes
#include <QString>

namespace spikestream {

	/*! Holds information about and data associated with a particular pattern.
		Patterns can be injected into neuron groups in Nemo. */
	class Pattern {
		public:
			Pattern();
			~Pattern();
			QString getName(){ return name; }

		private:
			//===================  VARIABLES  ===================
			QString name;

	};

}

#endif//PATTERN_H
