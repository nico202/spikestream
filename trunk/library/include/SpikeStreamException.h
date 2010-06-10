#ifndef SPIKESTREAMEXCEPTION_H
#define SPIKESTREAMEXCEPTION_H

#include <QString>
namespace spikestream {

	/*! SpikeStream specific exception. Other SpikeStream exception classes inherit from this class. */
	class SpikeStreamException {
		public:
			SpikeStreamException(QString details = QString("No details available"));
			virtual ~SpikeStreamException();
			virtual QString getMessage();

		protected:
			QString type;

		private:
			QString details;
	};

}

#endif // SPIKESTREAMEXCEPTION_H
