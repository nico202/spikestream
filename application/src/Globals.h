#ifndef GLOBALS_H
#define GLOBALS_H

//Qt includes
#include <QString>

class Globals {
  public:
    static QString getSpikeStreamRoot();
    static QString getWorkingDirectory();
    friend class SpikeStreamMainWindow;

  private:
    //=====================  VARIABLES ============================
    static QString spikeStreamRoot;
    static QString workingDirectory;


    //======================  METHODS  ============================
    static void setSpikeStreamRoot(QString rootDir);
    static void setWorkingDirectory(QString workingDir);

};

#endif // GLOBALS_H
