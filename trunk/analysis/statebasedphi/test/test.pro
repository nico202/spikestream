SPIKESTREAM_ROOT_DIR = ../../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TARGET = teststatebasedphi

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/bin

QT += xml opengl sql

CONFIG += qtestlib console


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/testlibrary/include \
				$${SPIKESTREAM_ROOT_DIR}/analysis/statebasedphi/src/database \
				$${SPIKESTREAM_ROOT_DIR}/analysis/statebasedphi/src/analysis \
				$${SPIKESTREAM_ROOT_DIR}/analysis/statebasedphi/src/models


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lstatebasedphi -L$${SPIKESTREAM_ROOT_DIR}/plugins/analysis -lspikestreamtest -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	LIBS += -lstatebasedphi0 -L$${SPIKESTREAM_ROOT_DIR}/plugins/analysis -lspikestreamtest0 -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/lib
}


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


