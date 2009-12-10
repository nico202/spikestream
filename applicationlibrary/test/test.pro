TARGET = testspikestreamapplication

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

INCLUDEPATH += .src \
		../src \
		../src/exceptions \
		../src/nrm

LIBS += -lspikestreamapplication -L$(SPIKESTREAM_ROOT)/lib

CONFIG += qtestlib

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


