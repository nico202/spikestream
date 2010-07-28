SPIKESTREAM_ROOT_DIR = ../../../

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TARGET = testnemo

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

CONFIG += qtestlib console


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src ../src/model
unix {
	INCLUDEPATH += /usr/local/include/nemo
}
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/nemo/include
}


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lnemo
}
win32 {
	LIBS += -lnemo -L$${SPIKESTREAM_ROOT_DIR}/extlib/nemo/lib
}


#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
			src/TestNemo.h

SOURCES += src/Main.cpp \
			src/TestRunner.cpp \
			src/TestNemo.cpp



