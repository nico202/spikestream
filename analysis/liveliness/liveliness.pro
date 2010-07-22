SPIKESTREAM_ROOT_DIR = ../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = liveliness

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/analysis

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src \
				src/database \
				src/analysis \
				src/models \
				src/views \
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
}


#=================  ANALYSIS  ===================
HEADERS = src/analysis/Cluster.h \
			src/analysis/LivelinessTimeStepThread.h \
			src/analysis/WeightlessLivelinessAnalyzer.h
SOURCES = src/analysis/Cluster.cpp \
			src/analysis/LivelinessTimeStepThread.cpp \
			src/analysis/WeightlessLivelinessAnalyzer.cpp

#====================  GUI  =====================
HEADERS += src/gui/LivelinessWidget.h \
			src/gui/LivelinessExportDialog.h \
			src/gui/LivelinessGraphDialog.h
SOURCES += src/gui/LivelinessWidget.cpp \
			src/gui/LivelinessExportDialog.cpp \
			src/gui/LivelinessGraphDialog.cpp

#==================  DATABASE  ===================
HEADERS += src/database/LivelinessDao.h
SOURCES += src/database/LivelinessDao.cpp

#==================  MODELS  =====================
HEADERS += src/models/LivelinessFullResultsModel.h \
			src/models/LivelinessSpectrogramData.h
SOURCES += src/models/LivelinessFullResultsModel.cpp \
			src/models/LivelinessSpectrogramData.cpp

#==================  VIEWS  =====================
HEADERS += src/views/LivelinessFullResultsTableView.h
SOURCES += src/views/LivelinessFullResultsTableView.cpp



