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
#include "SpikeStreamMainWindow.h"
using namespace spikestream;

//Qt includes
#include <QString>

namespace spikestream {
    class Globals {
		public:
			static bool archiveLoaded();
			static void cleanUp();
			static unsigned int getAnalysisID(const QString& analysisName);
			static AnalysisDao* getAnalysisDao();
			static Archive* getArchive();
			static ArchiveDao* getArchiveDao();
			static EventRouter* getEventRouter() {return eventRouter; }
			static Network* getNetwork();
			static NetworkDao* getNetworkDao();
			static NetworkDisplay* getNetworkDisplay();
			static QString getSpikeStreamRoot();
			static QString getWorkingDirectory();
			static float getVertexSize();
			static bool isArchivePlaying();
			static bool isAnalysisLoaded(const QString& analysisName);
			static bool isRendering();
			static bool networkLoaded();
			static void setAnalysisID(const QString& analysisName, unsigned int id);
			static void setArchivePlaying(bool archivePlaying);
			static void setVertexSize(float vertexSize);
			static void startTimer();
			static unsigned int timeElapsed();

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

			/*! Stores the id for each analysis type.
				An ID of 0 indicates that no analysis is loaded for that type. */
			static QHash<QString, unsigned int> analysisMap;

			/*! Class controlling how the current network is displayed */
			static NetworkDisplay* networkDisplay;

			/*! Router for application-wide events */
			static EventRouter* eventRouter;

			/*! Records when rendering is in progress */
			static bool rendering;

			/*! The time at which the timer starts */
			static unsigned int timerStart_sec;

			/*! Is archive playback in progress */
			static bool archivePlaying;

			/*! Size of vertices in the 3D Network Viewer */
			static float vertexSize;

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

}

#endif // GLOBALS_H
