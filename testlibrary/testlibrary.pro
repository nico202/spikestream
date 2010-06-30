SPIKESTREAM_ROOT_DIR = ..

TEMPLATE = lib

TARGET = spikestreamtest

win32 {
	# Add a copy of the libary to the bin directory
	target.path = $${SPIKESTREAM_ROOT_DIR}/bin
	INSTALLS += target
}

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/lib

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += release qtestlib

QT += sql

INCLUDEPATH += include 	$${SPIKESTREAM_ROOT_DIR}/library/include

unix{
	LIBS += -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	LIBS += -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/lib
}

HEADERS = include/TestDao.h

SOURCES = src/TestDao.cpp





