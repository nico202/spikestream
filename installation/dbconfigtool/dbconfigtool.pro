SPIKESTREAM_ROOT_DIR = ../..

TEMPLATE = app

TARGET = dbconfigtool

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

unix{
	LIBS += -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestream0
}


INCLUDEPATH += src  \
				$${SPIKESTREAM_ROOT_DIR}/library/include

CONFIG += release exceptions

QT += sql xml

HEADERS = src/DBConfigMainWindow.h \
			src/DBDetailsWidget.h \
			src/SuccessWidget.h

SOURCES += src/Main.cpp \
			src/DBConfigMainWindow.cpp \
			src/DBDetailsWidget.cpp \
			src/SuccessWidget.cpp

