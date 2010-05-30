SPIKESTREAM_ROOT_DIR = ../..

TEMPLATE = lib

TARGET = nemowrapper

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/simulation

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
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/nemo/include
	LIBS += -lnemo -L$${SPIKESTREAM_ROOT_DIR}/extlib/nemo/lib
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0
}


#----------------------------------------------#
#-----               GUI                  -----#
#----------------------------------------------#
HEADERS += src/gui/NemoWidget.h \
			src/gui/NemoParametersDialog.h
SOURCES += src/gui/NemoWidget.cpp \
			src/gui/NemoParametersDialog.cpp


#----------------------------------------------#
#-----              Model                 -----#
#----------------------------------------------#
HEADERS += src/model/NemoWrapper.h \
			src/model/NemoLoader.h
SOURCES += src/model/NemoWrapper.cpp \
			src/model/NemoLoader.cpp

