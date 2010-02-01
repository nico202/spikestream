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

#====================  GUI  =====================
HEADERS = src/gui/LivelinessWidget.h
SOURCES = src/gui/LivelinessWidget.cpp

#==================  ANALYSIS  ===================
#HEADERS +=
#SOURCES += src/analysis/AnalysisRunner.cpp \


#==================  DATABASE  ===================
#HEADERS += src/database/StateBasedPhiAnalysisDao.h
#SOURCES += src/database/StateBasedPhiAnalysisDao.cpp

#==================  MODELS  =====================
#HEADERS +=
#SOURCES +=

#==================  VIEWS  =====================
#HEADERS += src/views/FullResultsTableView.h
#SOURCES += src/views/FullResultsTableView.cpp



