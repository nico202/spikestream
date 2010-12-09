SPIKESTREAM_ROOT_DIR = ../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = nrmimporter

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/networks

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += include \
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
#---                  GUI                   ---#
#----------------------------------------------#
HEADERS += include/NRMImportWidget.h
SOURCES += src/gui/NRMImportWidget.cpp


#----------------------------------------------#
#---                EXCEPTIONS              ---#
#----------------------------------------------#
HEADERS += include/NRMException.h
SOURCES += src/exceptions/NRMException.cpp


#----------------------------------------------#
#---                 MODEL                  ---#
#----------------------------------------------#
HEADERS += include/NRMNetwork.h \
			include/NRMConfigLoader.h \
			include/NRMTrainingLoader.h \
			include/NRMConstants.h \
			include/NRMLayer.h \
			include/NRMNeuralLayer.h \
			include/NRMInputLayer.h \
			include/NRMConnection.h \
			include/NRMNeuron.h \
			include/NRMRandom.h \
			include/NRMFileLoader.h \
			include/NRMDataImporter.h \
			include/NRMDataSetImporter.h \
			include/NRMDataSet.h
SOURCES += src/model/NRMNetwork.cpp \
			src/model/NRMConfigLoader.cpp \
			src/model/NRMTrainingLoader.cpp \
			src/model/NRMLayer.cpp \
			src/model/NRMNeuralLayer.cpp \
			src/model/NRMInputLayer.cpp \
			src/model/NRMConnection.cpp \
			src/model/NRMNeuron.cpp \
			src/model/NRMRandom.cpp \
			src/model/NRMFileLoader.cpp \
			src/model/NRMDataImporter.cpp \
			src/model/NRMDataSetImporter.cpp \
			src/model/NRMDataSet.cpp 


