SPIKESTREAM_ROOT_DIR = ../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = app

TARGET = dbconfigtool

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

QT += sql xml


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src  \
				$${SPIKESTREAM_ROOT_DIR}/library/include


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix{
	LIBS += -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestream0
}


#----------------------------------------------#
#---                 FILES                  ---#
#----------------------------------------------#
HEADERS = src/DBConfigMainWindow.h \
			src/DBDetailsWidget.h \
			src/SuccessWidget.h

SOURCES += src/Main.cpp \
			src/DBConfigMainWindow.cpp \
			src/DBDetailsWidget.cpp \
			src/SuccessWidget.cpp

