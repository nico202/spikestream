SPIKESTREAM_ROOT_DIR = ../../..

TARGET = testliveliness

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

INCLUDEPATH += src \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/testlibrary/include \
				$${SPIKESTREAM_ROOT_DIR}/analysis/liveliness/src/database \
				$${SPIKESTREAM_ROOT_DIR}/analysis/liveliness/src/analysis

unix {
	LIBS += -lliveliness -L$${SPIKESTREAM_ROOT_DIR}/plugins/analysis -lspikestreamtest -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include
	LIBS += -lliveliness0 -L$${SPIKESTREAM_ROOT_DIR}/plugins/analysis -lspikestreamtest0 -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/lib
}

QT += sql

CONFIG += release console qtestlib

#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
			src/TestLivelinessDao.h \
			src/TestWeightlessLivelinessAnalyzer.h \
			src/LivelinessDaoDuck.h

SOURCES += src/Main.cpp \
			src/TestRunner.cpp \
			src/TestLivelinessDao.cpp \
			src/TestWeightlessLivelinessAnalyzer.cpp \
			src/LivelinessDaoDuck.cpp


