TEMPLATE = lib

TARGET = statebasedphi

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/plugins/analysis

OBJECTS_DIR = build/objects

MOC_DIR = build/moc


CONFIG += debug \
	  warn_on \
	  qt \
	  opengl \
	  thread \
	  exceptions \
	  stl

QT += xml opengl qt3support sql

INCLUDEPATH += src \
		src/database \
		$(SPIKESTREAM_ROOT)/library/include \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/widgets \
		$(SPIKESTREAM_ROOT)/applicationlibrary/src/dialogs

LIBS += -L$(SPIKESTREAM_ROOT)/lib -lspikestream -lspikestreamapplication


HEADERS = src/gui/StateBasedPhiWidget.h \
	    src/database/StateBasedPhiAnalysisDao.h \
	    src/gui/StateBasedPhiParameterDialog.h \
	    src/analysis/AnalysisRunner.h \
	    src/analysis/AnalysisTimeStepThread.h

SOURCES = src/gui/StateBasedPhiWidget.cpp \
	    src/database/StateBasedPhiAnalysisDao.cpp \
	    src/gui/StateBasedPhiParameterDialog.cpp \
	    src/analysis/AnalysisRunner.cpp \
	    src/analysis/AnalysisTimeStepThread.cpp



