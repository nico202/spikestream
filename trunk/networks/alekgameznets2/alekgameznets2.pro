SPIKESTREAM_ROOT_DIR = ../..

TEMPLATE = lib

TARGET = alekgameznets2

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/networks

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += release thread exceptions

QT += xml opengl sql

INCLUDEPATH += src \
				src/gui \
				src/model \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/database \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/widgets \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/dialogs

unix{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication -lspikestream
}
win32{
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication0 -lspikestream0 -lgmp -L$${SPIKESTREAM_ROOT_DIR}/extlib/gmp/lib
}

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
