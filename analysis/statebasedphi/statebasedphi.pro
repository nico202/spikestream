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
	    src/database/StateBasedPhiAnalysisDao.h

SOURCES = src/gui/StateBasedPhiWidget.cpp \
	    src/database/StateBasedPhiAnalysisDao.cpp



