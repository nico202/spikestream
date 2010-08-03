SPIKESTREAM_ROOT_DIR = ../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = nemowrapper

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/simulation

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src src/model src/gui \
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
unix{
	LIBS += -lnemo -L$${SPIKESTREAM_ROOT_DIR}/lib  -lspikestreamapplication -lspikestream
}
win32{
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0
	LIBS += -lnemo -L$${SPIKESTREAM_ROOT_DIR}/extlib/nemo/lib
}


#----------------------------------------------#
#-----               GUI                  -----#
#----------------------------------------------#
HEADERS += src/gui/NemoWidget.h \
			src/gui/NemoParametersDialog.h
SOURCES += src/gui/NemoWidget.cpp \
			src/gui/NemoParametersDialog.cpp


#----------------------------------------------#
#-----              Model                 -----#
#----------------------------------------------#
HEADERS += src/model/NemoWrapper.h \
			src/model/NemoLoader.h
SOURCES += src/model/NemoWrapper.cpp \
			src/model/NemoLoader.cpp

