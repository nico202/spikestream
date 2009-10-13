#ifndef TESTQAPPLICATION_H
#define TESTQAPPLICATION_H

#include <QApplication>

class testQApplication : public QApplication {
    public:
            testQApplication(int & argc, char ** argv);
            ~testQApplication();

};




#endif // TESTQAPPLICATION_H
