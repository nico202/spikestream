SPIKESTREAM_ROOT_DIR = ../..

TARGET = testspikestreamlibrary

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

QT += sql xml

CONFIG += debug qtestlib console


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src ../include $${SPIKESTREAM_ROOT_DIR}/testlibrary/include
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include
}


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
LIBS += -lgmpxx
unix {
	LIBS += -lspikestreamtest -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	LIBS += -lspikestreamtest0 -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/extlib/gmp/lib
}


#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
			src/TestDatabaseDao.h \
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
			src/TestDatabaseDao.cpp \
			src/TestNetworkDao.cpp \
			src/TestNetworkDaoThread.cpp \
			src/TestXMLParameterParser.cpp \
			src/TestNetwork.cpp \
			src/TestNeuronGroup.cpp \
			src/TestArchiveDao.cpp \
			src/TestAnalysisDao.cpp \
			src/TestUtil.cpp \
			src/TestWeightlessNeuron.cpp \



