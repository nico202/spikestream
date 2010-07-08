# Create the directory structure
CreateDirectory $INSTDIR\bin

# Configuration files
File ${SPIKESTREAM_ROOT}\spikestream.config.template

# Executable files
File /oname=bin\spikestream.exe ${SPIKESTREAM_ROOT}\bin\spikestream.exe
File /oname=bin\dbconfigtool.exe ${SPIKESTREAM_ROOT}\bin\dbconfigtool.exe

# Core library files
File /oname=bin\spikestream0.dll ${SPIKESTREAM_ROOT}\bin\spikestream0.dll
File /oname=bin\spikestreamapplication0.dll ${SPIKESTREAM_ROOT}\bin\spikestreamapplication0.dll

# Plugins
File /r /x .svn /x *.a /x *d4.dll ${SPIKESTREAM_ROOT}\plugins

# Qt library files
File /oname=bin\QtCore4.dll ${SPIKESTREAM_ROOT}\bin\QtCore4.dll
File /oname=bin\QtGui4.dll ${SPIKESTREAM_ROOT}\bin\QtGui4.dll
File /oname=bin\QtOpenGL4.dll ${SPIKESTREAM_ROOT}\bin\QtOpenGL4.dll
File /oname=bin\QtSql4.dll ${SPIKESTREAM_ROOT}\bin\QtSql4.dll

# Other library files
File /oname=bin\cudart32_30_14.dll ${SPIKESTREAM_ROOT}\bin\cudart32_30_14.dll
File /oname=bin\cudartemu32_30_14.dll ${SPIKESTREAM_ROOT}\bin\cudartemu32_30_14.dll
File /oname=bin\nemo.dll ${SPIKESTREAM_ROOT}\bin\nemo.dll
File /oname=bin\nemo_base.dll ${SPIKESTREAM_ROOT}\bin\nemo_base.dll
File /oname=bin\nemo_cuda.dll ${SPIKESTREAM_ROOT}\bin\nemo_cuda.dll
File /oname=bin\qwt5.dll ${SPIKESTREAM_ROOT}\bin\qwt5.dll

# Images
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.xpm
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.png
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.ico

# Database files
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.sql

# Document files
File /r /x .svn ${SPIKESTREAM_ROOT}\*.pdf

