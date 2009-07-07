#ifndef GLOBALS_H
#define GLOBALS_H

//Qt includes
#include <QString>

class Globals {
  public:
    static QString getRootDirectory();
    friend class SpikeStreamMainWindow;

  private:
    //=====================  VARIABLES ============================
    static QString rootDirectory;


    //======================  METHODS  ============================
    static void setRootDirectory(QString rootDir);

};

#endif // GLOBALS_H
