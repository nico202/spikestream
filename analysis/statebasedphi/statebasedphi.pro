TEMPLATE = lib

TARGET = statebasedphi

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

#====================  GUI  =====================
HEADERS = src/gui/StateBasedPhiWidget.h \
		src/gui/StateBasedPhiParameterDialog.h \
		src/gui/ProgressWidget.h \
		src/gui/StateBasedPhiExportDialog.h
SOURCES = src/gui/StateBasedPhiWidget.cpp \
		src/gui/StateBasedPhiParameterDialog.cpp \
		src/gui/ProgressWidget.cpp \
		src/gui/StateBasedPhiExportDialog.cpp

#==================  ANALYSIS  ===================
HEADERS += src/analysis/AnalysisRunner.h \
		src/analysis/PhiAnalysisTimeStepThread.h \
		src/analysis/PhiCalculator.h \
		src/analysis/SubsetManager.h \
		src/analysis/Subset.h \
		src/analysis/ProbabilityTable.h
SOURCES += src/analysis/AnalysisRunner.cpp \
		src/analysis/PhiAnalysisTimeStepThread.cpp \
		src/analysis/PhiCalculator.cpp \
		src/analysis/SubsetManager.cpp \
		src/analysis/Subset.cpp \
		src/analysis/ProbabilityTable.cpp

#==================  DATABASE  ===================
HEADERS += src/database/StateBasedPhiAnalysisDao.h
SOURCES += src/database/StateBasedPhiAnalysisDao.cpp

#==================  MODELS  =====================
HEADERS += src/models/FullResultsModel.h \
		src/models/Complex.h
SOURCES += src/models/FullResultsModel.cpp \
		src/models/Complex.cpp

#==================  VIEWS  =====================
HEADERS += src/views/FullResultsTableView.h
SOURCES += src/views/FullResultsTableView.cpp



