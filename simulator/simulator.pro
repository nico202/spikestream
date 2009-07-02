TEMPLATE = app

TARGET = spikestreamsimulator

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

CONFIG += debug \
          warn_on

HEADERS = include/ClassLoader.h \
            include/ConnectionHolder.h \
            include/Debug.h \
            include/DeviceManager.h \
            include/Neuron.h \
            include/NeuronTaskHolder.h \
            include/PatternManager.h \
            include/SimulationClock.h \
            include/SpikeStreamSimulation.h \
            include/Synapse.h \
            include/TaskHolder.h \
            src/TCPSynchronizedClient.h \
            src/TCPSynchronizedServer.h \
            src/UDPSynchronizedClient.h \
            src/UDPSynchronizedServer.h

SOURCES = src/ClassLoader.cpp \
            src/DeviceManager.cpp \
            src/Main.cpp \
            src/Neuron.cpp \
            src/NeuronTaskHolder.cpp \
            src/PatternManager.cpp \
            src/SimulationClock.cpp \
            src/SpikeStreamSimulation.cpp \
            src/Synapse.cpp \
            src/TCPSynchronizedClient.cpp \
            src/TCPSynchronizedServer.cpp \
            src/TaskHolder.cpp \
            src/UDPSynchronizedClient.cpp \
            src/UDPSynchronizedServer.cpp

INCLUDEPATH += include \
                src \
                $(SPIKESTREAM_ROOT)/library/include \
                /usr/include/mysql \
                /usr/local/include/mysql++

LIBS += -lmysqlpp -lpvm3 -L$(SPIKESTREAM_ROOT)/lib -lspikestream


