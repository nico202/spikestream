SPIKESTREAM_ROOT_DIR = ../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = transferentropy

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/analysis

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src \
				src/analysis \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include

#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestream -lspikestreamapplication
}
win32 {
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0

	# Add a copy of the plugin to the bin directory
	target.path = $${SPIKESTREAM_ROOT_DIR}/bin
	INSTALLS += target
}


#=================  ANALYSIS  ===================
HEADERS += src/analysis/TransferEntropyTimeStepThread.h \
			src/analysis/TransferEntropyCalculator.h
SOURCES += src/analysis/TransferEntropyTimeStepThread.cpp \
			src/analysis/TransferEntropyCalculator.cpp

#====================  GUI  =====================
HEADERS += src/gui/TransferEntropyWidget.h 
SOURCES += src/gui/TransferEntropyWidget.cpp 

#==================  DATABASE  ===================

#==================  MODELS  =====================

#==================  VIEWS  =====================



