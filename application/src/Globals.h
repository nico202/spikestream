#ifndef GLOBALS_H
#define GLOBALS_H

//SpikeStream includes
#include "NetworkDao.h"
#include "Network.h"
using namespace spikestream;

//Qt includes
#include <QString>

class Globals {
  public:
    static void cleanUp();
    static Network* getNetwork();
    static NetworkDao* getNetworkDao();
    static QString getSpikeStreamRoot();
    static QString getWorkingDirectory();
    friend class SpikeStreamMainWindow;

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
    Network* network;


    //======================  METHODS  ============================
    static void setNetworkDao(NetworkDao* networkDao);
    static void setNetwork(Network* network);
    static void setSpikeStreamRoot(QString rootDir);
    static void setWorkingDirectory(QString workingDir);

};

#endif // GLOBALS_H
