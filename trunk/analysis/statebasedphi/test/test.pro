SPIKESTREAM_ROOT_DIR = ../../..

TARGET = teststatebasedphi

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

INCLUDEPATH += src \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/testlibrary/include \
				$${SPIKESTREAM_ROOT_DIR}/analysis/statebasedphi/src/database \
				$${SPIKESTREAM_ROOT_DIR}/analysis/statebasedphi/src/analysis \
				$${SPIKESTREAM_ROOT_DIR}/analysis/statebasedphi/src/models

unix {
	LIBS += -lstatebasedphi -L$${SPIKESTREAM_ROOT_DIR}/plugins/analysis -lspikestreamtest -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include
	LIBS += -lstatebasedphi0 -L$${SPIKESTREAM_ROOT_DIR}/plugins/analysis -lspikestreamtest0 -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/lib
}

QT += xml opengl qt3support sql

CONFIG += release qtestlib console

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


