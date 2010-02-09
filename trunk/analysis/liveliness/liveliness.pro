TEMPLATE = lib

TARGET = liveliness

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/plugins/analysis

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += debug warn_on qt opengl thread exceptions stl

QT += xml opengl qt3support sql

#----------------------------------------------#
#---                INCLUDE                 ---#
#----------------------------------------------#
INCLUDEPATH += src \
		src/database \
		src/analysis \
		src/models \
		src/views \
		$(SPIKESTREAM_ROOT)/library/include \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/analysis \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/widgets \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/dialogs \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/models \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/views
win32 {
	INCLUDEPATH += $(SPIKESTREAM_ROOT)/extlib/gmp/include
}

#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lgmpxx -L$(SPIKESTREAM_ROOT)/lib -lspikestream -lspikestreamapplication
}
win32 {
	LIBS += -lgmp -L$(SPIKESTREAM_ROOT)/extlib/gmp/lib
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


