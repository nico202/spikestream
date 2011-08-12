# Settings that apply across the SpikeStream build

#----------------------------------------------#
#---            DEBUG OR RELEASE            ---#
#----------------------------------------------#
DEBUG_MODE = false
contains (DEBUG_MODE, true) {
	CONFIG += debug
}
else{
	CONFIG += release
}


#----------------------------------------------#
#---             CONSOLE OUTPUT             ---#
#----------------------------------------------#
CONFIG += console

#----------------------------------------------#
#---       INCLUDE AND LIBRARY PATHS        ---#
#----------------------------------------------#
unix:!macx {
	# Qwt
	INCLUDEPATH += /usr/local/qwt-5.2.1-svn/include
	LIBS += -lqwt -L/usr/local/qwt-5.2.1-svn/lib

	# GMP
	LIBS += -lgmpxx
}
win32 {
	# Qwt
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/qwt/include
	contains (DEBUG_MODE, true) {
		LIBS += -lqwtd5 -L$${SPIKESTREAM_ROOT_DIR}/extlib/qwt/lib
	}
	else{
		LIBS += -lqwt5 -L$${SPIKESTREAM_ROOT_DIR}/extlib/qwt/lib
	}


	# GMP
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include
	LIBS += -lgmpxx -lgmp -L$${SPIKESTREAM_ROOT_DIR}/extlib/gmp/lib

	# Boost
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/boost/include
}
macx {
        # Qwt
        INCLUDEPATH += /usr/local/qwt-5.2.2-svn/include
        LIBS += -lqwt -L/usr/local/qwt-5.2.2-svn/lib

        # GMP
        INCLUDEPATH += /opt/local/include
        LIBS += -lgmpxx -lgmp -L/opt/local/lib

        CONFIG += x86
}

#----------------------------------------------#
#---		 GENERAL BUILD SETTINGS         ---#
#----------------------------------------------#
VERSION = 0.2

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += thread exceptions


#----------------------------------------------#
#---                DEFINES                 ---#
#----------------------------------------------#
unix:!macx {
	DEFINES += LINUX32_SPIKESTREAM
}
win32 {
	DEFINES += WIN32_SPIKESTREAM
}
macx {
	DEFINES += MAC32_SPIKESTREAM
}

