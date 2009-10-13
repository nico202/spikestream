#ifndef GLOBALS_H
#define GLOBALS_H

//SpikeStream includes
#include "AnalysisDao.h"
#include "Archive.h"
#include "ArchiveWidget_V2.h"
#include "NetworksWidget.h"
#include "EventRouter.h"
#include "ArchiveDao.h"
#include "NetworkDao.h"
#include "Network.h"
#include "NetworkDisplay.h"
#include "NetworkViewer_V2.h"
using namespace spikestream;

//Qt includes
#include <QString>

class Globals {
  public:
    static bool archiveLoaded();
    static void cleanUp();
    static AnalysisDao* getAnalysisDao();
    static Archive* getArchive();
    static ArchiveDao* getArchiveDao();
    static EventRouter* getEventRouter() {return eventRouter; }
    static Network* getNetwork();
    static NetworkDao* getNetworkDao();
    static NetworkDisplay* getNetworkDisplay();
    static QString getSpikeStreamRoot();
    static QString getWorkingDirectory();
    static bool isRendering();
    static bool networkLoaded();

    /* Friend classes that are allowed to change the state of Globals
	by accessing private setter methods*/
    friend class SpikeStreamMainWindow;
    friend class spikestream::NetworksWidget;
    friend class spikestream::NetworkViewer_V2;
    friend class spikestream::ArchiveWidget_V2;

  private:
    //=====================  VARIABLES ============================
    /*! Wrapper for SpikeStreamAnalysis database */
    static AnalysisDao* analysisDao;

    /*! Wrapper for the SpikeStreamNetwork database */
    static NetworkDao* networkDao;

    /*! Wrapper for SpikeStreamArchive database */
    static ArchiveDao* archiveDao;

    /*! Corresponds to the root of the whole spikestream directory,
	environment variable SPIKESTREAMROOT */
    static QString spikeStreamRoot;

    /*! Default location for loading and saving files */
    static QString workingDirectory;

    /*! The current network that is being viewed, edited, etc. */
    static Network* network;

    /*! The current loaded archive */
    static Archive* archive;

    /*! Class controlling how the current network is displayed */
    static NetworkDisplay* networkDisplay;

    /*! Router for application-wide events */
    static EventRouter* eventRouter;

    /*! Records when rendering is in progress */
    static bool rendering;


    //======================  METHODS  ============================
    static void setAnalysisDao(AnalysisDao* analysisDao);
    static void setArchive(Archive* archive);
    static void setArchiveDao(ArchiveDao* archiveDao);
    static void setEventRouter(EventRouter* eventRouter);
    static void setNetworkDisplay(NetworkDisplay* networkDisplay);
    static void setNetworkDao(NetworkDao* networkDao);
    static void setNetwork(Network* network);
    static void setRendering(bool rendering);
    static void setSpikeStreamRoot(QString rootDir);
    static void setWorkingDirectory(QString workingDir);

};

#endif // GLOBALS_H
