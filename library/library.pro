SPIKESTREAM_ROOT_DIR = ..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = spikestream

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/lib

QT += sql xml


#----------------------------------------------#
#---          INSTALLATION LOCATION         ---#
#----------------------------------------------#
unix {
	target.path = /usr/local/lib
	INSTALLS += target
}
win32 {
	# Add a copy of the libary to the bin directory
	target.path = $${SPIKESTREAM_ROOT_DIR}/bin
	INSTALLS += target
}


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += include


#----------------------------------------------#
#---                  src                   ---#
#----------------------------------------------#
HEADERS = include/GlobalVariables.h \
			include/PerformanceTimer.h \
			include/RGBColor.h \
			include/Box.h \
			include/Util.h

SOURCES = src/PerformanceTimer.cpp \
			src/Box.cpp \
			src/RGBColor.cpp \
			src/Util.cpp

#----------------------------------------------#
#---               database                 ---#
#----------------------------------------------#
HEADERS += include/AbstractDao.h \
			include/DatabaseDao.h \
			include/DBInfo.h \
			include/NetworkDao.h \
			include/NetworkDaoThread.h \
			include/ArchiveDao.h \
			include/AnalysisDao.h
SOURCES += src/database/DBInfo.cpp \
			src/database/DatabaseDao.cpp \
			src/database/AbstractDao.cpp \
			src/database/NetworkDao.cpp \
			src/database/NetworkDaoThread.cpp \
			src/database/ArchiveDao.cpp \
			src/database/AnalysisDao.cpp

#----------------------------------------------#
#---             exceptions                 ---#
#----------------------------------------------#
HEADERS += include/SpikeStreamDBException.h \
			include/SpikeStreamIOException.h \
			include/SpikeStreamException.h \
			include/SpikeStreamXMLException.h \
			include/NumberConversionException.h \
			include/SpikeStreamAnalysisException.h \
			include/SpikeStreamSimulationException.h
SOURCES += src/exceptions/SpikeStreamDBException.cpp \
			src/exceptions/SpikeStreamIOException.cpp \
			src/exceptions/SpikeStreamException.cpp \
			src/exceptions/SpikeStreamXMLException.cpp \
			src/exceptions/NumberConversionException.cpp \
			src/exceptions/SpikeStreamAnalysisException.cpp \
			src/exceptions/SpikeStreamSimulationException.cpp

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
			include/NeuronType.h \
			include/Point3D.h \
			include/SynapseType.h \
			include/ParameterInfo.h
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
			src/model/NeuronType.cpp \
			src/model/Point3D.cpp \
			src/model/SynapseType.cpp \
			src/model/ParameterInfo.cpp

#----------------------------------------------#
#---                  xml                   ---#
#----------------------------------------------#
HEADERS += include/XMLParameterParser.h
SOURCES += src/xml/XMLParameterParser.cpp


#----------------------------------------------#
#---                 file                   ---#
#----------------------------------------------#

HEADERS += include/ConfigLoader.h \
			include/ConfigEditor.h
SOURCES += src/file/ConfigLoader.cpp \
			src/file/ConfigEditor.cpp
