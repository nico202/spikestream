SPIKESTREAM_ROOT_DIR = ../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = connectionmatriximporter

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/networks

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src src/model \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication -lspikestream
}
win32{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication0 -lspikestream0
}


#----------------------------------------------#
#---                 FILES                  ---#
#----------------------------------------------#
HEADERS = src/gui/ConnectionMatrixImporterWidget.h \
			src/model/NeuronGroupBuilder.h

SOURCES = src/gui/ConnectionMatrixImporterWidget.cpp \
			src/model/NeuronGroupBuilder.cpp

