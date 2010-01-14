#ifndef ARCHIVEPLAYERTHREAD_H
#define ARCHIVEPLAYERTHREAD_H

//SpikeStream includes
#include "ArchiveDao.h"
#include "DBInfo.h"
#include "RGBColor.h"
using namespace spikestream;

//Qt includes
#include <QString>
#include <QHash>
#include <QMutex>
#include <QThread>


namespace spikestream {

    class ArchivePlayerThread : public QThread {
	Q_OBJECT

	public:
	    ArchivePlayerThread(DBInfo dbInfo);
	    ~ArchivePlayerThread();

	    QString getErrorMessage();
	    bool isError();
	    void play(unsigned int startTimeStep, unsigned int archiveID, unsigned int frameRate);
	    void step(unsigned int startTimeStep, unsigned int archiveID);
	    void setFrameRate(unsigned int frameRate);
	    void stop();

	signals:
	    void archiveTimeStepChanged();

	private slots:
	    void run();

	private:
	    //===========================  VARIABLES  ============================
	    /*! Set to true to exit the run method */
	    bool stopThread;

	    /*! Flags that an error has occurred */
	    bool error;

	    /*! Message associated with an error */
	    QString errorMessage;

	    /*! The ID of the archive that is being played */
	    unsigned int archiveID;

	    /*! Time step to start playing from */
	    unsigned int startTimeStep;

	    /*! Interval between the presentation of each time step in milliseconds. */
	    unsigned int updateInterval_ms;

	    /*! Data access class wrapping the archive databse */
	    ArchiveDao* archiveDao;

	    /*! Information about the archive database */
	    DBInfo archiveDBInfo;

	    /*! Mutex controlling access to variables */
	    QMutex mutex;

	    /*! In step mode the run method only advances by one step */
	    bool stepMode;


	    //============================  METHODS  =============================
	    void clearError();
	    QHash<unsigned int, RGBColor*>* getNeuronColorMap(int timeStep);
	    void setError(const QString& errMsg);
    };

}

#endif//ARCHIVEPLAYERTHREAD_H

