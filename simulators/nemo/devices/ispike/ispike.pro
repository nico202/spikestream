SPIKESTREAM_ROOT_DIR = ../../../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = ispike

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/simulation/nemodevices

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src/gui src/managers src/models src/views \
				$${SPIKESTREAM_ROOT_DIR}/simulators/nemo/include \
				$${SPIKESTREAM_ROOT_DIR}/library/include \
				$${SPIKESTREAM_ROOT_DIR}/applicationlibrary/include
unix {
	INCLUDEPATH += /usr/local/include
}
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/ispike/include
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
	LIBS += -llibiSpike -L$${SPIKESTREAM_ROOT_DIR}/extlib/ispike/lib
}
macx {
	LIBS += -lnemo -lspikestreamapplication -lspikestream
}


#----------------------------------------------#
#-----               GUI                  -----#
#----------------------------------------------#
HEADERS += src/gui/ISpikeWidget.h
SOURCES += src/gui/ISpikeWidget.cpp


#----------------------------------------------#
#-----               MODELS               -----#
#----------------------------------------------#
HEADERS += src/models/ChannelModel.h
SOURCES += src/models/ChannelModel.cpp


#----------------------------------------------#
#-----               VIEWS               -----#
#----------------------------------------------#
HEADERS += src/views/ChannelTableView.h
SOURCES += src/views/ChannelTableView.cpp


#----------------------------------------------#
#-----              MANAGERS              -----#
#----------------------------------------------#
HEADERS += src/managers/ISpikeManager.h
SOURCES += src/managers/ISpikeManager.cpp

