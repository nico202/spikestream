SPIKESTREAM_ROOT_DIR = ../..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = nemowrapper

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/plugins/simulation

QT += xml opengl sql

#----------------------------------------------#
#---          INSTALLATION LOCATION         ---#
#----------------------------------------------#
unix:!macx {
	target.path = /usr/local/lib
	INSTALLS += target
}
win32 {
	# Add a copy of the libary to the bin directory
	target.path = $${SPIKESTREAM_ROOT_DIR}/bin
	INSTALLS += target
}
macx {
	# Add a copy of the libary to the lib directory
	target.path = /usr/lib
	INSTALLS += target
}


#----------------------------------------------#
#---              INCLUDE PATH              ---#
#----------------------------------------------#
INCLUDEPATH += include \
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
	LIBS += -L$${SPIKESTREAM_ROOT_DIR}/lib -lspikestreamapplication0 -lspikestream0
	LIBS += -lnemo -L$${SPIKESTREAM_ROOT_DIR}/extlib/nemo/lib
}
macx {
	LIBS += -lnemo -lspikestreamapplication -lspikestream
}


#----------------------------------------------#
#-----             Dialogs                -----#
#----------------------------------------------#
HEADERS += include/NemoParametersDialog.h
SOURCES += src/dialogs/NemoParametersDialog.cpp


#----------------------------------------------#
#-----               GUI                  -----#
#----------------------------------------------#
HEADERS += include/NemoWidget.h \
			include/ExperimentLoaderWidget.h
SOURCES += src/gui/NemoWidget.cpp \
			src/gui/ExperimentLoaderWidget.cpp

#----------------------------------------------#
#-----              Managers              -----#
#----------------------------------------------#
HEADERS += include/PatternManager.h
SOURCES += src/managers/PatternManager.cpp


#----------------------------------------------#
#-----              Model                 -----#
#----------------------------------------------#
HEADERS += include/NemoWrapper.h \
			include/NemoLoader.h \
			include/STDPFunctions.h \
			include/StandardSTDPFunction.h \
			include/AbstractSTDPFunction.h \
			include/Pattern.h \
			include/RasterModel.h \
			include/StepSTDPFunction.h
SOURCES += src/model/NemoWrapper.cpp \
			src/model/NemoLoader.cpp \
			src/model/STDPFunctions.cpp \
			src/model/StandardSTDPFunction.cpp \
			src/model/AbstractSTDPFunction.cpp \
			src/model/Pattern.cpp \
			src/model/RasterModel.cpp \
			src/model/StepSTDPFunction.cpp

#----------------------------------------------#
#-----               View                 -----#
#----------------------------------------------#
HEADERS += include/RasterView.h
SOURCES += src/views/RasterView.cpp


