# Needs to have the same include paths etc. as the application library

TEMPLATE = app

TARGET = spikestream

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

unix{
    LIBS += -lspikestream -lspikestreamapplication -L$(SPIKESTREAM_ROOT)/lib
}
win32{
	LIBS += -L$(SPIKESTREAM_ROOT)/lib -lspikestreamapplication0 -lspikestream0 -lgmp -L$(SPIKESTREAM_ROOT)/extlib/gmp/lib
}


INCLUDEPATH += src \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/analysis \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/exceptions \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/nrm \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/dialogs \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/widgets \
		$(SPIKESTREAM_ROOT)/library/include

CONFIG += release thread exceptions

QT += xml opengl qt3support sql

SOURCES += src/Main.cpp
