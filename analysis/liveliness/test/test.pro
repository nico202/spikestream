TARGET = testliveliness

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

INCLUDEPATH += src \
		$(SPIKESTREAM_ROOT)/library/include \
		$(SPIKESTREAM_ROOT)/testlibrary/include \
		$(SPIKESTREAM_ROOT)/analysis/liveliness/src/database \
		$(SPIKESTREAM_ROOT)/analysis/liveliness/src/analysis

LIBS += -lliveliness -L$(SPIKESTREAM_ROOT)/plugins/analysis -lspikestreamtest -L$(SPIKESTREAM_ROOT)/lib

QT += sql

CONFIG += qtestlib

#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
		src/TestLivelinessDao.h

SOURCES += src/Main.cpp \
		src/TestRunner.cpp \
		src/TestLivelinessDao.cpp


