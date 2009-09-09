TARGET = testspikestream

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

INCLUDEPATH += src \
		../include

LIBS += -lspikestream -L$(SPIKESTREAM_ROOT)/lib  -lmysqlpp

QT += sql

CONFIG += qtestlib

#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
	    src/TestDao.h \
	    src/TestNetworkDao.h \
	    src/TestNetworkDaoThread.h

SOURCES += src/Main.cpp \
	    src/TestRunner.cpp \
	    src/TestDao.cpp \
	    src/TestNetworkDao.cpp \
	    src/TestNetworkDaoThread.cpp



