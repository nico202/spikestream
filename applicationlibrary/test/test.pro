# Test functions for the SpikeStream Application library

SPIKESTREAM_ROOT_DIR = ../..

TARGET = testspikestreamapplication

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += debug console qtestlib


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += ../include


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lspikestreamapplication -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	LIBS += -lspikestreamapplication0 -L$${SPIKESTREAM_ROOT_DIR}/lib
}


#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestNRMConfigLoader.h \
			src/TestRunner.h \
			src/TestNRMTrainingLoader.h \
			src/TestNRMRandom.h \
			src/TestNRMConnection.h \
			src/TestNRMDataSetImporter.h \
			src/TestNRMDataImporter.h

SOURCES += src/Main.cpp \
			src/TestRunner.cpp \
			src/TestNRMConfigLoader.cpp \
			src/TestNRMTrainingLoader.cpp \
			src/TestNRMRandom.cpp \
			src/TestNRMConnection.cpp \
			src/TestNRMDataSetImporter.cpp \
			src/TestNRMDataImporter.cpp


