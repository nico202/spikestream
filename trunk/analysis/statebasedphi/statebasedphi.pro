TEMPLATE = lib

TARGET = statebasedphi

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/plugins/analysis

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

HEADERS = src/gui/StateBasedPhiWidget.h

SOURCES = src/gui/StateBasedPhiWidget.cpp

CONFIG += plugin thread

INCLUDEPATH += src \
                $(SPIKESTREAM_ROOT)/library/include \


LIBS += -L$(SPIKESTREAM_ROOT)/lib -lspikestream

