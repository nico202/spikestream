#ifndef ARCHIVEWIDGET_V2
#define ARCHIVEWIDGET_V2

namespace spikestream{

    class ArchiveWidget_V2 : public QWidget {
	public:
	    ArchiveWidget_V2(QWidget* parent);
	    ~ArchiveWidget_V2();

	signals:
	    void archiveChanged();

	private slots:
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

	    const static int idCol = 0;
	    const static int netIDCol = 1;
	    const static int dateCol = 2;
	    const static int descCol = 3;
	    const static int loadButCol = 4;
	    const static int delButCol = 5;

	    //========================  METHODS  =========================
	    void buildTransportControls();

    };
}

#endif//ARCHIVEWIDGET_V2

