SPIKESTREAM_ROOT_DIR = ../../../

TARGET = testnemo

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

INCLUDEPATH += src ../src/model
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/nemo/include
}

win32 {
	LIBS += -lnemo -L$${SPIKESTREAM_ROOT_DIR}/extlib/nemo/lib
}

CONFIG += release qtestlib console


#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
			src/TestNemo.h

SOURCES += src/Main.cpp \
			src/TestRunner.cpp \
			src/TestNemo.cpp



