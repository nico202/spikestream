SPIKESTREAM_ROOT_DIR = ../../../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = temporalcodingexpt1

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/simulation/nemoexperiments

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src/gui src/managers \
				$${SPIKESTREAM_ROOT_DIR}/simulators/nemo/include \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include
unix {
	INCLUDEPATH += /usr/local/include
}
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/nemo/include
}


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix:!macx {
	LIBS += -lnemo -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication -lspikestream
}
win32{
	LIBS += -lnemowrapper0 -L$${SPIKESTREAM_ROOT_DIR}/plugins/simulation/
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0
	LIBS += -lnemo -L$${SPIKESTREAM_ROOT_DIR}/extlib/nemo/lib
}
macx {
	LIBS += -lnemo -lspikestreamapplication -lspikestream
}


#----------------------------------------------#
#-----               GUI                  -----#
#----------------------------------------------#
HEADERS += src/gui/TemporalCodingExptWidget.h
SOURCES += src/gui/TemporalCodingExptWidget.cpp


#----------------------------------------------#
#-----              Managers              -----#
#----------------------------------------------#
HEADERS += src/managers/TemporalCodingExptManager.h
SOURCES += src/managers/TemporalCodingExptManager.cpp

