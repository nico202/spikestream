TEMPLATE = lib

TARGET = spikestreamsimulator

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/lib

OBJECTS_DIR = build/objects

CONFIG += debug \
          warn_on

SOURCES = $(SPIKESTREAM_ROOT)/simulator/src/ClassLoader.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/DeviceManager.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/Main.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/Neuron.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/NeuronTaskHolder.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/PatternManager.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/SimulationClock.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/SpikeStreamSimulation.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/Synapse.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/TCPSynchronizedClient.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/TCPSynchronizedServer.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/TaskHolder.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/UDPSynchronizedClient.cpp \
            $(SPIKESTREAM_ROOT)/simulator/src/UDPSynchronizedServer.cpp


INCLUDEPATH += $(SPIKESTREAM_ROOT)/library/include \
                $(SPIKESTREAM_ROOT)/simulator/include \
                $(SPIKESTREAM_ROOT)/simulator/src \
                /usr/include/mysql \
                /usr/local/include/mysql++

LIBS += -lmysqlpp -lpvm3 -L$(SPIKESTREAM_ROOT)/lib -lspikestream


