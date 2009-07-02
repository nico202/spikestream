TEMPLATE = lib

TARGET = spikestream

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/lib

OBJECTS_DIR = build/objects

CONFIG += debug \
          warn_on

HEADERS = include/ConfigLoader.h \
            include/ConnectionType.h \
            include/DBInterface.h \
            include/DeviceMessages.h \
            include/DeviceTypes.h \
            include/GlobalVariables.h \
            include/LogWriter.h \
            include/NeuronGroupType.h \
            include/PatternTypes.h \
            include/PerformanceTimer.h \
            include/PVMMessages.h \
            include/SimulationTypes.h \
            include/Utilities.h

SOURCES = src/ConfigLoader.cpp \
            src/ConnectionType.cpp \
            src/DBInterface.cpp \
            src/DeviceTypes.cpp \
            src/LogWriter.cpp \
            src/NeuronGroupType.cpp \
            src/PatternTypes.cpp \
            src/PerformanceTimer.cpp \
            src/Utilities.cpp
			
INCLUDEPATH += include \
                /usr/include/mysql \
                /usr/local/include/mysql++


