TEMPLATE = lib

TARGET = spikestreamtest

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/lib

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += qtestlib

QT += sql


INCLUDEPATH += include 	$(SPIKESTREAM_ROOT)/library/include

HEADERS = include/TestDao.h

SOURCES = src/TestDao.cpp





