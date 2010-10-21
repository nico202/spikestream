#ifndef EVENTROUTER_H
#define EVENTROUTER_H

//Qt includes
#include <QObject>

namespace spikestream {

	/*! Class held within globals that enables other classes to listen for application
		wide events, such as reload, etc. */
	class EventRouter : public QObject {
		Q_OBJECT

		public:

		public slots:
			//Archive slots
			void archiveListChangedSlot() { emit archiveListChangedSignal(); }
			void archiveChangedSlot() { emit archiveChangedSignal(); }

			//Analysis slots
			void analysisStarted() { emit analysisStartedSignal(); emit analysisNotRunningSignal(false); }
			void analysisStopped() { emit analysisStoppedSignal(); emit analysisNotRunningSignal(true); }

			//Network slots
			void networkChangedSlot() { emit networkChangedSignal(); }
			void networkListChangedSlot() { emit networkListChangedSignal(); }
			void networkDisplayChangedSlot() { emit networkDisplayChangedSignal(); }
			void neuronGroupDisplayChangedSlot() { emit neuronGroupDisplayChangedSignal(); }
			void weightsChangedSlot() { emit weightsChangedSignal(); }

			//Other slots
			void reloadSlot() { emit reloadSignal(); }


		signals:
			//Analysis signals
			void analysisNotRunningSignal(bool isNotRunning);//FIXME HORRIBLE
			void analysisStartedSignal();
			void analysisStoppedSignal();

			//Archive signals
			void archiveChangedSignal();
			void archiveListChangedSignal();

			//OpenGL signals
			void moveBackwardSignal();
			void moveForwardSignal();
			void moveUpSignal();
			void moveDownSignal();
			void moveLeftSignal();
			void moveRightSignal();
			void resetViewSignal();
			void rotateUpSignal();
			void rotateDownSignal();
			void rotateLeftSignal();
			void rotateRightSignal();

			//Network signals
			void networkChangedSignal();
			void networkDisplayChangedSignal();
			void networkListChangedSignal();
			void networkViewChangedSignal();
			void neuronGroupDisplayChangedSignal();
			void weightsChangedSignal();

			//Other signals
			void reloadSignal();


		private slots:
			//OpenGL slots
			void moveBackwardSlot() { emit moveBackwardSignal(); emit networkViewChangedSignal(); }
			void moveForwardSlot() { emit moveForwardSignal(); emit networkViewChangedSignal(); }
			void moveUpSlot() { emit moveUpSignal(); emit networkViewChangedSignal(); }
			void moveDownSlot() { emit moveDownSignal(); emit networkViewChangedSignal(); }
			void moveLeftSlot() { emit moveLeftSignal(); emit networkViewChangedSignal(); }
			void moveRightSlot() { emit moveRightSignal(); emit networkViewChangedSignal(); }
			void resetViewSlot() { emit resetViewSignal(); emit networkViewChangedSignal(); }
			void rotateUpSlot() { emit rotateUpSignal(); emit networkViewChangedSignal(); }
			void rotateDownSlot() { emit rotateDownSignal(); emit networkViewChangedSignal(); }
			void rotateLeftSlot() { emit rotateLeftSignal(); emit networkViewChangedSignal(); }
			void rotateRightSlot() { emit rotateRightSignal(); emit networkViewChangedSignal(); }
	};

}

#endif//EVENTROUTER_H

