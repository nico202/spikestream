TARGET = testspikestream

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

INCLUDEPATH += .src \
                ../src \
                ../src/exceptions \
                ../src/nrm
LIBS += -lgmp

CONFIG += qtestlib

#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestNRMConfigLoader.h \
            src/TestRunner.h \
            src/TestNRMTrainingLoader.h \
            src/TestNRMRandom.h \
            src/TestNRMConnection.h

SOURCES += src/Main.cpp \
            src/TestRunner.cpp \
            src/TestNRMConfigLoader.cpp \
            src/TestNRMTrainingLoader.cpp \
            src/TestNRMRandom.cpp \
            src/TestNRMConnection.cpp

#----------------------------------------------#
#---          Files used in tests           ---#
#----------------------------------------------#
SOURCES += ../src/nrm/NRMNetwork.cpp \
    ../src/nrm/NRMConfigLoader.cpp \
    ../src/nrm/NRMTrainingLoader.cpp \
    ../src/exceptions/NRMException.cpp \
    ../src/nrm/NRMLayer.cpp \
    ../src/nrm/NRMNeuralLayer.cpp \
    ../src/nrm/NRMInputLayer.cpp \
    ../src/nrm/NRMConnection.cpp \
    ../src/nrm/NRMNeuron.cpp \
    ../src/nrm/NRMRandom.cpp
