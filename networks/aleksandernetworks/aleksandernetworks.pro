SPIKESTREAM_ROOT_DIR = ../..

TEMPLATE = lib

TARGET = aleksandernetworks

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/networks

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += release thread exceptions

QT += xml opengl sql

INCLUDEPATH += src \
				src/model \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/widgets \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/database \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/dialogs

unix{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication -lspikestream
}
win32{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication0 -lspikestream0
}

HEADERS = src/gui/AleksanderNetworksWidget.h \
			src/model/AleksanderNetworksBuilder.h

SOURCES = src/gui/AleksanderNetworksWidget.cpp \
			src/model/AleksanderNetworksBuilder.cpp

