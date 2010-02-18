TEMPLATE = lib

TARGET = tononinetworks

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/plugins/networks

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += release thread exceptions

QT += xml opengl qt3support sql

INCLUDEPATH += src \
				src/model \
				$(SPIKESTREAM_ROOT)/library/include \
				$(SPIKESTREAM_ROOT)/applicationlibrary/src \
				$(SPIKESTREAM_ROOT)/applicationlibrary/src/widgets \
				$(SPIKESTREAM_ROOT)/applicationlibrary/src/dialogs

unix{
	LIBS += -L$(SPIKESTREAM_ROOT)/lib  -lspikestreamapplication -lspikestream
}
win32{
	LIBS += -L$(SPIKESTREAM_ROOT)/lib  -lspikestreamapplication0 -lspikestream0
	INCLUDEPATH += $(SPIKESTREAM_ROOT)/extlib/gmp/include
}

HEADERS = src/gui/TononiNetworksWidget.h \
			src/model/TononiNetworkBuilder.h

SOURCES = src/gui/TononiNetworksWidget.cpp \
			src/model/TononiNetworkBuilder.cpp




