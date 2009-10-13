# Needs to have the same include paths etc. as the application library

TEMPLATE = app

TARGET = spikestream

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

LIBS += -lspikestreamapplication -L$(SPIKESTREAM_ROOT)/lib

INCLUDEPATH += src \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/analysis \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/exceptions \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/nrm \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/dialogs \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/widgets \
		$(SPIKESTREAM_ROOT)/library/include \
		/usr/include/mysql \
		/usr/local/include/mysql++ \
		/usr/local/qwt/include

CONFIG += debug \
	  warn_on \
	  qt \
	  opengl \
	  thread \
	  exceptions \
	  stl

QT += xml opengl qt3support sql

SOURCES += src/Main.cpp
