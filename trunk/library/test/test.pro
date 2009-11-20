TARGET = testspikestream

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

INCLUDEPATH += src \
		../include \
		$(SPIKESTREAM_ROOT)/testlibrary/include

LIBS += -lspikestream -lspikestreamapplication -lspikestreamtest -L$(SPIKESTREAM_ROOT)/lib

QT += sql xml

CONFIG += qtestlib

#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
	    src/TestNetworkDao.h \
	    src/TestNetworkDaoThread.h \
	    src/TestXMLParameterParser.h \
	    src/TestNetwork.h \
	    src/TestNeuronGroup.h \
	    src/TestArchiveDao.h \
	    src/TestAnalysisDao.h \
	    src/TestUtil.h \
	    src/TestWeightlessNeuron.h

SOURCES += src/Main.cpp \
	    src/TestRunner.cpp \
	    src/TestNetworkDao.cpp \
	    src/TestNetworkDaoThread.cpp \
	    src/TestXMLParameterParser.cpp \
	    src/TestNetwork.cpp \
	    src/TestNeuronGroup.cpp \
	    src/TestArchiveDao.cpp \
	    src/TestAnalysisDao.cpp \
	    src/TestUtil.cpp \
	    src/TestWeightlessNeuron.cpp


