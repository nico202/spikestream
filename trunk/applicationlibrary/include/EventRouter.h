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

		//Analysis slots
		void analysisStarted() { emit analysisStartedSignal(); emit analysisNotRunningSignal(false); }
		void analysisStopped() { emit analysisStoppedSignal(); emit analysisNotRunningSignal(true); }
		void archiveTimeStepChangedSlot() { emit archiveTimeStepChangedSignal(); }
		void networkChangedSlot() { emit networkChangedSignal(); }
		void reloadSlot() { emit reloadSignal(); }
		void networkDisplayChangedSlot() { emit networkDisplayChangedSignal(); }

	signals:
		//Analysis signals
		void analysisNotRunningSignal(bool isNotRunning);//FIXME HORRIBLE
		void analysisStartedSignal();
		void analysisStoppedSignal();

		//Archive signals
		void archiveChangedSignal();
		void archiveListChangedSignal();
		void archiveTimeStepChangedSignal();

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

		void networkChangedSignal();
		void networkDisplayChangedSignal();
		void networkListChangedSignal();
		void networkViewChangedSignal();
		void reloadSignal();

	private slots:
		void archiveChangedSlot() { emit archiveChangedSignal(); }

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

		void networkListChangedSlot() { emit networkListChangedSignal(); }

	};

}

#endif//EVENTROUTER_H

