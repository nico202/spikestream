SPIKESTREAM_ROOT_DIR = ..

TEMPLATE = app

TARGET = spikestream

VERSION = 0.2

RC_FILE = spikestream.rc

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

OBJECTS_DIR = build/objects

CONFIG += console debug thread exceptions

QT += xml opengl sql


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix{
	LIBS += -lspikestream -lspikestreamapplication -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0
}


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include $${SPIKESTREAM_ROOT_DIR}/library/include



#----------------------------------------------#
#---                 FILES                  ---#
#----------------------------------------------#
SOURCES += src/Main.cpp
