SPIKESTREAM_ROOT_DIR = ../..

TEMPLATE = lib

TARGET = statebasedphi

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/analysis

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += dll debug thread exceptions

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src src/database src/analysis src/models src/views \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include
unix {
	INCLUDEPATH += /usr/local/qwt-5.2.1-svn/include
}
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include $${SPIKESTREAM_ROOT_DIR}/extlib/qwt/include
}


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
LIBS += -lgmpxx
unix {
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestream -lspikestreamapplication
	LIBS += -lqwt -L/usr/local/qwt-5.2.1-svn/lib
}
win32 {
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/extlib/gmp/lib
	LIBS += -lqwt5 -L$${SPIKESTREAM_ROOT_DIR}/extlib/qwt/lib
}


#====================  GUI  =====================
HEADERS = src/gui/StateBasedPhiWidget.h \
			src/gui/StateBasedPhiExportDialog.h \
			src/gui/StateBasedPhiGraphDialog.h
SOURCES = src/gui/StateBasedPhiWidget.cpp \
			src/gui/StateBasedPhiExportDialog.cpp \
			src/gui/StateBasedPhiGraphDialog.cpp


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
			src/models/Complex.h \
			src/models/StateBasedPhiSpectrogramData.h
SOURCES += src/models/FullResultsModel.cpp \
			src/models/Complex.cpp \
			src/models/StateBasedPhiSpectrogramData.cpp

#==================  VIEWS  =====================
HEADERS += src/views/FullResultsTableView.h
SOURCES += src/views/FullResultsTableView.cpp



