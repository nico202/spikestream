SPIKESTREAM_ROOT_DIR = ..

TEMPLATE = app

TARGET = spikestream

VERSION = 0.2

RC_FILE = spikestream.rc

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

OBJECTS_DIR = build/objects

unix{
	LIBS += -lspikestream -lspikestreamapplication -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0 -lgmp -L$${SPIKESTREAM_ROOT_DIR}/extlib/gmp/lib
}


INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include $${SPIKESTREAM_ROOT_DIR}/library/include

CONFIG += release thread exceptions

QT += xml opengl sql

SOURCES += src/Main.cpp
