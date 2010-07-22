SPIKESTREAM_ROOT_DIR = ..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = app

TARGET = spikestream

RC_FILE = spikestream.rc

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

CONFIG += console

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
