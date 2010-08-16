#ifndef SPIKESTREAMTHREAD_H
#define SPIKESTREAMTHREAD_H

//Qt includes
#include <QThread>

namespace spikestream {

	/*! Implements basic functionality used by all SpikeStream threads */
	class SpikeStreamThread : public QThread {
		Q_OBJECT

		public:
			SpikeStreamThread();
			virtual ~SpikeStreamThread();
			void clearError();
			virtual QString getErrorMessage();
			virtual bool isError();
			virtual void run() = 0;
			virtual void stop();


		protected:
			//======================  VARIABLES  ==========================
			/*! Message associated with an error */
			QString errorMessage;

			/*! Used to cancel the operation */
			bool stopThread;

			/*! Set to true when an error has occurred */
			bool error;


			//=====================  METHODS  ======================
			void setError(const QString& errorMessage);
	};
}

#endif//SPIKESTREAMTHREAD_H
