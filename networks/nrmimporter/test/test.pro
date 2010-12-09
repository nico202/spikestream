SPIKESTREAM_ROOT_DIR = ../../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TARGET = testnrmimporter

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

CONFIG += console qtestlib


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += ../include $${SPIKESTREAM_ROOT_DIR}/library/include


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lspikestreamapplication -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib
	LIBS +=  -lnrmimporter -L$${SPIKESTREAM_ROOT_DIR}/plugins/networks
}
win32 {
	LIBS +=  -lnrmimporter0 -L$${SPIKESTREAM_ROOT_DIR}/plugins/networks
	LIBS += -lspikestreamapplication0 -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/lib
}


#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestNRMConfigLoader.h \
			src/TestRunner.h \
			src/TestNRMTrainingLoader.h \
			src/TestNRMRandom.h \
			src/TestNRMConnection.h \
			src/TestNRMDataSetImporter.h \
			src/TestNRMDataImporter.h

SOURCES += src/Main.cpp \
			src/TestRunner.cpp \
			src/TestNRMConfigLoader.cpp \
			src/TestNRMTrainingLoader.cpp \
			src/TestNRMRandom.cpp \
			src/TestNRMConnection.cpp \
			src/TestNRMDataSetImporter.cpp \
			src/TestNRMDataImporter.cpp


