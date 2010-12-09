# Create the directory structure
CreateDirectory $INSTDIR\bin
CreateDirectory $INSTDIR\bin\sqldrivers
CreateDirectory $INSTDIR\log

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
File /oname=bin\QtCore4.dll C:\Qt\4.7.1\bin\QtCore4.dll
File /oname=bin\QtGui4.dll C:\Qt\4.7.1\bin\QtGui4.dll
File /oname=bin\QtOpenGL4.dll C:\Qt\4.7.1\bin\QtOpenGL4.dll
File /oname=bin\QtSql4.dll C:\Qt\4.7.1\bin\QtSql4.dll
File /oname=bin\QtXml4.dll C:\Qt\4.7.1\bin\QtXml4.dll
File /oname=bin\sqldrivers\qsqlmysql4.dll C:\Qt\4.7.1\plugins\sqldrivers\qsqlmysql4.dll

#MinGW libraries
File /oname=bin\libgcc_s_dw2-1.dll C:\MinGW\bin\libgcc_s_dw2-1.dll
File /oname=bin\libstdc++-6.dll C:\MinGW\bin\libstdc++-6.dll
File /oname=bin\mingwm10.dll C:\MinGW\bin\mingwm10.dll

# Other library files
File /oname=bin\cudart32_31_9.dll C:\CUDA\bin\cudart32_31_9.dll
File /oname=bin\nemo.dll ${SPIKESTREAM_ROOT}\bin\nemo.dll
File /oname=bin\nemo_base.dll ${SPIKESTREAM_ROOT}\bin\nemo_base.dll
File /oname=bin\nemo_cuda.dll ${SPIKESTREAM_ROOT}\bin\nemo_cuda.dll
File /oname=bin\nemo_cpu.dll ${SPIKESTREAM_ROOT}\bin\nemo_cpu.dll
File /oname=bin\qwt5.dll ${SPIKESTREAM_ROOT}\bin\qwt5.dll
File /oname=bin\libmySQL.dll C:\MySQL\bin\libmySQL.dll

# Images
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.xpm
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.png
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.ico
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.jpg

# Database files
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.sql

# Document files
File /r /x .svn ${SPIKESTREAM_ROOT}\*.pdf

