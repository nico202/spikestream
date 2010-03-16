SPIKESTREAM_ROOT_DIR = ../..

TEMPLATE = lib

TARGET = statebasedphi

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
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0 -lgmpxx -lgmp -L$${SPIKESTREAM_ROOT_DIR}/extlib/gmp/lib
}

#====================  GUI  =====================
HEADERS = src/gui/StateBasedPhiWidget.h \
			src/gui/StateBasedPhiExportDialog.h
SOURCES = src/gui/StateBasedPhiWidget.cpp \
			src/gui/StateBasedPhiExportDialog.cpp

#==================  ANALYSIS  ===================
HEADERS += src/analysis/PhiAnalysisTimeStepThread.h \
			src/analysis/PhiCalculator.h \
			src/analysis/SubsetManager.h \
			src/analysis/Subset.h \
			src/analysis/ProbabilityTable.h
SOURCES += src/analysis/PhiAnalysisTimeStepThread.cpp \
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



