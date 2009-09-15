#ifndef GLOBALS_H
#define GLOBALS_H

//SpikeStream includes
#include "NetworksWidget.h"
#include "EventRouter.h"
#include "NetworkDao.h"
#include "Network.h"
using namespace spikestream;

//Qt includes
#include <QString>

class Globals {
  public:
    static void cleanUp();
    static EventRouter* getEventRouter() {return eventRouter; }
    static Network* getNetwork();
    static NetworkDao* getNetworkDao();
    static QString getSpikeStreamRoot();
    static QString getWorkingDirectory();

    /* Friend classes that are allowed to change the state of Globals
	by accessing private setter methods*/
    friend class SpikeStreamMainWindow;
    friend class spikestream::NetworksWidget;

  private:
    //=====================  VARIABLES ============================
    /*! Wrapper for the SpikeStreamNetwork database */
    static NetworkDao* networkDao;

    /*! Corresponds to the root of the whole spikestream directory,
	environment variable SPIKESTREAMROOT */
    static QString spikeStreamRoot;

    /*! Default location for loading and saving files */
    static QString workingDirectory;

    /*! The current network that is being viewed, edited, etc. */
    static Network* network;

    /*! Router for application-wide events */
    static EventRouter* eventRouter;


    //======================  METHODS  ============================
    static void setEventRouter(EventRouter* eventRouter);
    static void setNetworkDao(NetworkDao* networkDao);
    static void setNetwork(Network* network);
    static void setSpikeStreamRoot(QString rootDir);
    static void setWorkingDirectory(QString workingDir);

};

#endif // GLOBALS_H
