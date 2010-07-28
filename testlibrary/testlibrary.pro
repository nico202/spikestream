SPIKESTREAM_ROOT_DIR = ..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = spikestreamtest

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/lib

CONFIG += qtestlib

QT += sql


#----------------------------------------------#
#---          INSTALLATION LOCATION         ---#
#----------------------------------------------#
unix {
	target.path = /usr/local/lib
	INSTALLS += target
}
win32 {
	# Add a copy of the libary to the bin directory
	target.path = $${SPIKESTREAM_ROOT_DIR}/bin
	INSTALLS += target
}


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += include 	$${SPIKESTREAM_ROOT_DIR}/library/include


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix{
	LIBS += -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	LIBS += -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/lib
}


#----------------------------------------------#
#---                 FILES                  ---#
#----------------------------------------------#
HEADERS = include/TestDao.h

SOURCES = src/TestDao.cpp





