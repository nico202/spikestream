TEMPLATE = app

TARGET = spikestreamarchiver

VERSION = 0.2

DESTDIR = ../bin

OBJECTS_DIR = build/objects

HEADERS = src/Debug.h \
            src/SpikeStreamArchiver.h

SOURCES = src/Main.cpp \
            src/SpikeStreamArchiver.cpp

INCLUDEPATH += $(SPIKESTREAM_ROOT)/library/include \
        /usr/include/mysql \
        /usr/local/include/mysql++

LIBS += -lmysqlpp -lpvm3 -L$(SPIKESTREAM_ROOT)/lib -lspikestream

