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

	signals:
	    void networkChangedSignal();
	    void networkDisplayChangedSignal();
	    void reloadSignal();

	private slots:
	    void networkChangedSlot() { emit networkChangedSignal(); emit networkDisplayChangedSignal(); }
	    void networkDisplayChangedSlot() { emit networkDisplayChangedSignal(); }
	    void reloadSlot() { emit reloadSignal(); }

    };

}

#endif//EVENTROUTER_H

