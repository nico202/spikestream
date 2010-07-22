SPIKESTREAM_ROOT_DIR = ../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = alekgameznets2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/networks

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src src/gui src/model \
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
HEADERS = src/gui/AlekGam2NetworksWidget.h \
			src/model/FullyConnectedNetworksBuilder.h \
			src/model/PartitionedNetworksBuilder.h \
			src/model/ModularNetworksBuilder.h \
			src/model/SensoryNetworksBuilder.h \
			src/model/MotorNetworksBuilder.h \
			src/model/TestPartitionedNetworksBuilder.h

SOURCES = src/gui/AlekGam2NetworksWidget.cpp \
			src/model/FullyConnectedNetworksBuilder.cpp \
			src/model/PartitionedNetworksBuilder.cpp \
			src/model/ModularNetworksBuilder.cpp \
			src/model/SensoryNetworksBuilder.cpp \
			src/model/MotorNetworksBuilder.cpp \
			src/model/TestPartitionedNetworksBuilder.cpp
