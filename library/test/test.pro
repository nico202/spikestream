TARGET = testspikestream

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

INCLUDEPATH += src \
		../include

LIBS += -lspikestream -L$(SPIKESTREAM_ROOT)/lib  -lmysqlpp

QT += sql xml

CONFIG += qtestlib

#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
	    src/TestDao.h \
	    src/TestNetworkDao.h \
	    src/TestNetworkDaoThread.h \
	    src/TestXMLParameterParser.h \
	    src/TestNetwork.h \
	    src/TestNeuronGroup.h \
	    src/TestArchiveDao.h

SOURCES += src/Main.cpp \
	    src/TestRunner.cpp \
	    src/TestDao.cpp \
	    src/TestNetworkDao.cpp \
	    src/TestNetworkDaoThread.cpp \
	    src/TestXMLParameterParser.cpp \
	    src/TestNetwork.cpp \
	    src/TestNeuronGroup.cpp \
	    src/TestArchiveDao.cpp


