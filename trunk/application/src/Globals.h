#ifndef GLOBALS_H
#define GLOBALS_H

//SpikeStream includes
#include "NetworkDao.h"

//Qt includes
#include <QString>

class Globals {
  public:
    static void cleanUp();
    static NetworkDao* getNetworkDao();
    static QString getSpikeStreamRoot();
    static QString getWorkingDirectory();
    friend class SpikeStreamMainWindow;

  private:
    //=====================  VARIABLES ============================
    static NetworkDao* networkDao;
    static QString spikeStreamRoot;
    static QString workingDirectory;


    //======================  METHODS  ============================
    static void setNetworkDao(NetworkDao* networkDao);
    static void setSpikeStreamRoot(QString rootDir);
    static void setWorkingDirectory(QString workingDir);

};

#endif // GLOBALS_H
