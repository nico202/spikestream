TEMPLATE = lib

TARGET = spikestream

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/lib

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += debug \
	  warn_on \
	  thread \
	  exceptions

QT += sql xml


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
	    include/Utilities.h \
	    include/Util.h

SOURCES = src/ConfigLoader.cpp \
	    src/ConnectionType.cpp \
	    src/DeviceTypes.cpp \
	    src/LogWriter.cpp \
	    src/NeuronGroupType.cpp \
	    src/PatternTypes.cpp \
	    src/PerformanceTimer.cpp \
	    src/Box.cpp \
	    src/Utilities.cpp \
	    src/RGBColor.cpp \
	    src/Util.cpp


#----------------------------------------------#
#---               database                 ---#
#----------------------------------------------#
HEADERS += include/AbstractDao.h \
	    include/DBInfo.h \
	    include/DBInterface.h \
	    include/NetworkDao.h \
	    include/NetworkDaoThread.h \
	    include/ArchiveDao.h \
	    include/AnalysisDao.h
SOURCES += src/database/DBInfo.cpp \
	    src/database/DBInterface.cpp \
	    src/database/AbstractDao.cpp \
	    src/database/NetworkDao.cpp \
	    src/database/NetworkDaoThread.cpp \
	    src/database/ArchiveDao.cpp \
	    src/database/AnalysisDao.cpp

#----------------------------------------------#
#---             exceptions                 ---#
#----------------------------------------------#
HEADERS += include/SpikeStreamDBException.h \
	    include/SpikeStreamException.h \
	    include/SpikeStreamXMLException.h \
	    include/NumberConversionException.h \
	    include/SpikeStreamAnalysisException.h
SOURCES += src/exceptions/SpikeStreamDBException.cpp \
	    src/exceptions/SpikeStreamException.cpp \
	    src/exceptions/SpikeStreamXMLException.cpp \
	    src/exceptions/NumberConversionException.cpp \
	    src/exceptions/SpikeStreamAnalysisException.cpp

#----------------------------------------------#
#---                model                   ---#
#----------------------------------------------#
HEADERS += include/Network.h \
	    include/NetworkInfo.h \
	    include/NeuronGroup.h \
	    include/NeuronGroupInfo.h \
	    include/Connection.h \
	    include/ConnectionGroup.h \
	    include/ConnectionGroupInfo.h \
	    include/Archive.h \
	    include/ArchiveInfo.h \
	    include/AnalysisInfo.h \
	    include/WeightlessNeuron.h \
	    include/Neuron.h \
	    include/Point3D.h
SOURCES += src/model/Network.cpp \
	    src/model/NetworkInfo.cpp \
	    src/model/NeuronGroup.cpp \
	    src/model/NeuronGroupInfo.cpp \
	    src/model/ConnectionGroup.cpp \
	    src/model/ConnectionGroupInfo.cpp \
	    src/model/Archive.cpp \
	    src/model/ArchiveInfo.cpp \
	    src/model/Connection.cpp \
	    src/model/AnalysisInfo.cpp \
	    src/model/WeightlessNeuron.cpp \
	    src/model/Neuron.cpp \
	    src/model/Point3D.cpp

#----------------------------------------------#
#---                  xml                   ---#
#----------------------------------------------#
HEADERS += include/XMLParameterParser.h
SOURCES += src/xml/XMLParameterParser.cpp




