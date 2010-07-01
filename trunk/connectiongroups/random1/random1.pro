SPIKESTREAM_ROOT_DIR = ../..

TEMPLATE = lib

TARGET = random1

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/connections

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += debug thread exceptions

QT += xml opengl sql

INCLUDEPATH += src \
				src/model \
				src/gui \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include

unix{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication -lspikestream
}
win32{
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication0 -lspikestream0
}

HEADERS = src/gui/Random1Widget.h \
			src/model/Random1BuilderThread.h

SOURCES = src/gui/Random1Widget.cpp \
			src/model/Random1BuilderThread.cpp

