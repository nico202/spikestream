TEMPLATE = lib

TARGET = spikestream

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/lib

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += debug \
	  warn_on \
	  thread

QT += sql


INCLUDEPATH += include \
		/usr/include/mysql \
		/usr/local/include/mysql++

HEADERS = include/ConfigLoader.h \
            include/ConnectionType.h \
            include/DeviceMessages.h \
            include/DeviceTypes.h \
            include/GlobalVariables.h \
            include/LogWriter.h \
            include/NeuronGroupType.h \
            include/PatternTypes.h \
            include/PerformanceTimer.h \
            include/PVMMessages.h \
	    include/RGBColor.h \
	    include/SimulationTypes.h \
	    include/Box.h \
	    include/Point3D.h \
            include/Utilities.h

SOURCES = src/ConfigLoader.cpp \
            src/ConnectionType.cpp \
	    src/DeviceTypes.cpp \
            src/LogWriter.cpp \
            src/NeuronGroupType.cpp \
            src/PatternTypes.cpp \
            src/PerformanceTimer.cpp \
	    src/Box.cpp \
            src/Utilities.cpp

#----------------------------------------------#
#---               database                 ---#
#----------------------------------------------#
HEADERS += include/AbstractDao.h \
	    include/DBInfo.h \
	    include/DBInterface.h \
	    include/NetworkDao.h \
	    include/NetworkDaoThread.h
SOURCES += src/database/DBInfo.cpp \
	    src/database/DBInterface.cpp \
	    src/database/AbstractDao.cpp \
	    src/database/NetworkDao.cpp \
	    src/database/NetworkDaoThread.cpp

#----------------------------------------------#
#---             exceptions                 ---#
#----------------------------------------------#
HEADERS += include/SpikeStreamDBException.h \
	    include/SpikeStreamException.h
SOURCES += src/exceptions/SpikeStreamDBException.cpp \
	    src/exceptions/SpikeStreamException.cpp

#----------------------------------------------#
#---                model                   ---#
#----------------------------------------------#
HEADERS += include/Network.h \
	    include/NetworkInfo.h \
	    include/NeuronGroup.h \
	    include/NeuronGroupInfo.h \
	    include/Connection.h \
	    include/ConnectionGroup.h \
	    include/ConnectionGroupInfo.h
SOURCES += src/model/Network.cpp \
	    src/model/NetworkInfo.cpp \
	    src/model/NeuronGroup.cpp \
	    src/model/NeuronGroupInfo.cpp \
	    src/model/ConnectionGroup.cpp \
	    src/model/ConnectionGroupInfo.cpp



