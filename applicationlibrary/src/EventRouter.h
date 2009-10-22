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
	    //Analysis slots
	    void analysisStarted() { emit analysisStartedSignal(); emit analysisNotRunningSignal(false); }
	    void analysisStopped() { emit analysisStoppedSignal(); emit analysisNotRunningSignal(true); }
	    void archiveTimeStepChangedSlot() { emit archiveTimeStepChangedSignal(); }

	signals:
	    //Analysis signals
	    void analysisNotRunningSignal(bool isNotRunning);//FIXME HORRIBLE
	    void analysisStartedSignal();
	    void analysisStoppedSignal();

	    //Archive signals
	    void archiveChangedSignal();
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
	    void reloadSignal();

	private slots:
	    void archiveChangedSlot() { emit archiveChangedSignal(); }

	    //OpenGL slots
	    void moveBackwardSlot() { emit moveBackwardSignal(); }
	    void moveForwardSlot() { emit moveForwardSignal(); }
	    void moveUpSlot() { emit moveUpSignal(); }
	    void moveDownSlot() { emit moveDownSignal(); }
	    void moveLeftSlot() { emit moveLeftSignal(); }
	    void moveRightSlot() { emit moveRightSignal(); }
	    void resetViewSlot() { emit resetViewSignal(); }
	    void rotateUpSlot() { emit rotateUpSignal(); }
	    void rotateDownSlot() { emit rotateDownSignal(); }
	    void rotateLeftSlot() { emit rotateLeftSignal(); }
	    void rotateRightSlot() { emit rotateRightSignal(); }

	    void networkChangedSlot() { emit networkChangedSignal(); }
	    void networkDisplayChangedSlot() { emit networkDisplayChangedSignal(); }
	    void networkListChangedSlot() { emit networkListChangedSignal(); }
	    void reloadSlot() { emit reloadSignal(); }

    };

}

#endif//EVENTROUTER_H

