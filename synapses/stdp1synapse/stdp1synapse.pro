TEMPLATE = lib

TARGET = stdp1synapse

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/lib

OBJECTS_DIR = build/objects

HEADERS = include/STDP1Synapse.h

SOURCES = src/STDP1Synapse.cpp

CONFIG += plugin

INCLUDEPATH += src \
                include \
                $(SPIKESTREAM_ROOT)/library/include \
                $(SPIKESTREAM_ROOT)/simulator/include


LIBS += -L$(SPIKESTREAM_ROOT)/lib -lspikestreamsimulator

