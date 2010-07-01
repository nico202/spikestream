SPIKESTREAM_ROOT_DIR = ../..

TEMPLATE = lib

TARGET = liveliness

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/analysis

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += debug thread exceptions

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
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include $${SPIKESTREAM_ROOT_DIR}/extlib/qwt/include
}

#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lgmpxx -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestream -lspikestreamapplication
}
win32 {
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0
	LIBS += -lgmpxx -lgmp -L$${SPIKESTREAM_ROOT_DIR}/extlib/gmp/lib
	LIBS += -lqwt5 -L$${SPIKESTREAM_ROOT_DIR}/extlib/qwt/lib
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



