#ifndef ARCHIVEWIDGET_V2
#define ARCHIVEWIDGET_V2

//SpikeStream includes
#include "ArchiveInfo.h"
#include "ArchivePlayerThread.h"
using namespace spikestream;

//Qt includes
#include <QWidget>
#include <QLayout>
#include <QHash>
#include <QPushButton>
#include <QLabel>

namespace spikestream{

    class ArchiveWidget_V2 : public QWidget {
	Q_OBJECT

	public:
	    ArchiveWidget_V2(QWidget* parent=0);
	    ~ArchiveWidget_V2();

	signals:
	    void archiveChanged();

	private slots:
	    void archiveTimeStepChanged();
	    void archivePlayerStopped();
	    void deleteArchive();
	    void loadArchive();
	    void loadArchiveList();
	    void rewindButtonPressed();
	    void playButtonToggled(bool on);
	    void stepButtonPressed();
	    void fastForwardButtonToggled(bool on);
	    void stopButtonPressed();
	    void frameRateComboChanged(int);


	private:
	    //======================  VARIABLES  =====================
	    /*! Keep the current list of archives in memory */
	    QHash<unsigned int, ArchiveInfo> archiveInfoMap;

	    /*! Layout used for organising widget. Need a reference to enable
		networks to be dynamically reloaded */
	    QGridLayout* gridLayout;

	    /*! Widget holding transport controls */
	    QWidget* transportControlWidget;

	    /*! Used to ignore button events when button state is changed programatically */
	    bool ignoreButton;

	    /*! Class responsible for playing the archive */
	    ArchivePlayerThread* archivePlayer;

	    /*! Records if the rewind button was used to stop the playback thread */
	    bool rewind;

	    QPushButton* playButton;
	    QPushButton* stopButton;
	    QPushButton* stepButton;
	    QPushButton* fastForwardButton;
	    QPushButton* rewindButton;
	    QComboBox* frameRateCombo;

	    QLabel* timeStepLabel;
	    QLabel* maxTimeStepLabel;


	    const static int idCol = 0;
	    const static int netIDCol = 1;
	    const static int dateCol = 2;
	    const static int descCol = 3;
	    const static int loadButCol = 4;
	    const static int delButCol = 5;

	    //========================  METHODS  =========================
	    void buildTransportControls();
	    void loadArchive(ArchiveInfo& archiveInfo);
	    void reset();
	    void rewindArchive();

    };
}

#endif//ARCHIVEWIDGET_V2

