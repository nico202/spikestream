SPIKESTREAM_ROOT_DIR = ../../../

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TARGET = testnemo

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

QT += sql

CONFIG += qtestlib console


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src ../src/model
unix {
	INCLUDEPATH += /usr/local/include \
					$${SPIKESTREAM_ROOT_DIR}/library/include \
					$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include \
					$${SPIKESTREAM_ROOT_DIR}/simulators/nemo/src/model
}
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/nemo/include \
					$${SPIKESTREAM_ROOT_DIR}/library/include \
					$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include \
					$${SPIKESTREAM_ROOT_DIR}/simulators/nemo/include
}


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lnemowrapper -L$${SPIKESTREAM_ROOT_DIR}/plugins/simulation -lnemo
	LIBS += -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	LIBS += -lnemowrapper0 -L$${SPIKESTREAM_ROOT_DIR}/plugins/simulation
	LIBS += -lnemo -L$${SPIKESTREAM_ROOT_DIR}/extlib/nemo/lib
	LIBS += -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/lib
}


#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
			src/TestNemoLibrary.h \
			src/TestNemoWrapper.h

SOURCES += src/Main.cpp \
			src/TestRunner.cpp \
			src/TestNemoLibrary.cpp \
			src/TestNemoWrapper.cpp




