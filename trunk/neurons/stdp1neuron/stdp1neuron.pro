TEMPLATE = lib

TARGET = stdp1neuron

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/lib

OBJECTS_DIR = build/objects

HEADERS = src/STDP1Neuron.h

SOURCES = src/STDP1Neuron.cpp

CONFIG += plugin



INCLUDEPATH += src \
                $(SPIKESTREAM_ROOT)/library/include \
                $(SPIKESTREAM_ROOT)/simulator/include \
                $(SPIKESTREAM_ROOT)/synapses/stdp1synapse/include


LIBS += -L$(SPIKESTREAM_ROOT)/lib -lspikestreamsimulator -lstdp1synapse

