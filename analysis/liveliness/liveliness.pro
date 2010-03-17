SPIKESTREAM_ROOT_DIR = ../..

TEMPLATE = lib

TARGET = liveliness

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/analysis

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += release thread exceptions

QT += xml opengl sql

#----------------------------------------------#
#---                INCLUDE                 ---#
#----------------------------------------------#
INCLUDEPATH += src \
				src/database \
				src/analysis \
				src/models \
				src/views \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/analysis \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/database \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/widgets \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/dialogs \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/models \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/src/views
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include
}

#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lgmpxx -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestream -lspikestreamapplication
}
win32 {
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0 -lgmp -L$${SPIKESTREAM_ROOT_DIR}/extlib/gmp/lib
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
			src/gui/LivelinessExportDialog.h
SOURCES += src/gui/LivelinessWidget.cpp \
			src/gui/LivelinessExportDialog.cpp

#==================  DATABASE  ===================
HEADERS += src/database/LivelinessDao.h
SOURCES += src/database/LivelinessDao.cpp

#==================  MODELS  =====================
HEADERS += src/models/LivelinessFullResultsModel.h
SOURCES += src/models/LivelinessFullResultsModel.cpp

#==================  VIEWS  =====================
HEADERS += src/views/LivelinessFullResultsTableView.h
SOURCES += src/views/LivelinessFullResultsTableView.cpp



