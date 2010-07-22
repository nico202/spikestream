# Settings that apply across the SpikeStream build

#----------------------------------------------#
#---            DEBUG OR RELEASE            ---#
#----------------------------------------------#
CONFIG += debug
#CONFIG += release


#----------------------------------------------#
#---       INCLUDE AND LIBRARY PATHS        ---#
#----------------------------------------------#
unix {
	# Qwt
	INCLUDEPATH += /usr/local/qwt-5.2.1-svn/include
	LIBS += -lqwt -L/usr/local/qwt-5.2.1-svn/lib
}
win32 {
	# Qwt
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include  $${SPIKESTREAM_ROOT_DIR}/extlib/qwt/include
	LIBS += -lqwt5 -L$${SPIKESTREAM_ROOT_DIR}/extlib/qwt/lib

	# GMP
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include
	LIBS += -lgmpxx -lgmp -L$${SPIKESTREAM_ROOT_DIR}/extlib/gmp/lib

	# Boost
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib
}


#----------------------------------------------#
#---		 GENERAL BUILD SETTINGS         ---#
#----------------------------------------------#
VERSION = 0.2

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

CONFIG += thread exceptions
