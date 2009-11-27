TARGET = teststatebasedphi

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

INCLUDEPATH += src \
		$(SPIKESTREAM_ROOT)/library/include \
		$(SPIKESTREAM_ROOT)/testlibrary/include \
		$(SPIKESTREAM_ROOT)/analysis/statebasedphi/src/database \
		$(SPIKESTREAM_ROOT)/analysis/statebasedphi/src/analysis

LIBS += -lstatebasedphi -L$(SPIKESTREAM_ROOT)/plugins/analysis -lspikestreamtest -L$(SPIKESTREAM_ROOT)/lib

QT += sql

CONFIG += qtestlib

#----------------------------------------------#
#---            Test Files                  ---#
#----------------------------------------------#
HEADERS += src/TestRunner.h \
	    src/TestStateBasedPhiAnalysisDao.h \
	    src/TestProbabilityTable.h \
	    src/TestSubset.h \
	    src/TestPhiCalculator.h \
	    src/PhiUtil.h \
	    src/TestSubsetManager.h \
	    src/StateBasedPhiAnalysisDaoDuck.h

SOURCES += src/Main.cpp \
	    src/TestRunner.cpp \
	    src/TestStateBasedPhiAnalysisDao.cpp \
	    src/TestProbabilityTable.cpp \
	    src/TestSubset.cpp \
	    src/TestPhiCalculator.cpp \
	    src/PhiUtil.cpp \
	    src/TestSubsetManager.cpp \
	    src/StateBasedPhiAnalysisDaoDuck.cpp


