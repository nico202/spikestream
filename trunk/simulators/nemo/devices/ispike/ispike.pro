SPIKESTREAM_ROOT_DIR = ../../../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = ispike

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/simulation/nemodevices

QT += xml opengl sql


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += src/dialogs src/gui src/managers src/models src/views \
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
	#QMAKE_CXXFLAGS += -Wl,-t -save-temps

	LIBS += -lnemowrapper0 -L$${SPIKESTREAM_ROOT_DIR}/plugins/simulation/
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0
	LIBS += -llibiSpike -L$${SPIKESTREAM_ROOT_DIR}/extlib/ispike/lib
	LIBS += -llibboost_regex-mgw45-mt-1_46 -llibboost_thread-mgw45-mt-1_46 -llibboost_system-mgw45-mt-1_46 -lws2_32
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/extlib/boost/lib
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
#-----             DIALOGS                -----#
#----------------------------------------------#
HEADERS += src/dialogs/AddChannelDialog.h
SOURCES += src/dialogs/AddChannelDialog.cpp


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

