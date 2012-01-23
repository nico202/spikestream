SPIKESTREAM_ROOT_DIR = ../../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TARGET = testtransferentropy

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

QT += sql

CONFIG += qtestlib console


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/testlibrary/include \
				$${SPIKESTREAM_ROOT_DIR}/analysis/transferentropy/src/analysis


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -ltransferentropy -L$${SPIKESTREAM_ROOT_DIR}/plugins/analysis -lspikestreamtest -lspikestreamapplication -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	LIBS += -ltransferentropy0 -L$${SPIKESTREAM_ROOT_DIR}/plugins/analysis -lspikestreamtest0 -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/lib
}


#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
			src/TestTransferEntropyCalculator.h 

SOURCES += src/Main.cpp \
			src/TestRunner.cpp \
			src/TestTransferEntropyCalculator.cpp 


