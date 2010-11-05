SPIKESTREAM_ROOT_DIR = ../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TARGET = testspikestreamlibrary

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

QT += sql xml

CONFIG += qtestlib console


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src ../include $${SPIKESTREAM_ROOT_DIR}/testlibrary/include


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lspikestreamtest -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	LIBS += -lspikestreamtest0 -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestream0
}


#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
			src/TestConnection.h \
			src/TestDatabaseDao.h \
			src/TestNetworkDao.h \
			src/TestNetworkDaoThread.h \
			src/TestXMLParameterParser.h \
			src/TestNetwork.h \
			src/TestNeuronGroup.h \
			src/TestArchiveDao.h \
			src/TestAnalysisDao.h \
			src/TestUtil.h \
			src/TestWeightlessNeuron.h \
			src/TestMemory.h

SOURCES += src/Main.cpp \
			src/TestRunner.cpp \
			src/TestConnection.cpp \
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
			src/TestMemory.cpp



