# Create the directory structure
CreateDirectory $INSTDIR\bin
CreateDirectory $INSTDIR\bin\sqldrivers
CreateDirectory $INSTDIR\log

# SpikeStream configuration files
File ${SPIKESTREAM_ROOT}\spikestream.config.template
File /oname=spikestream.config ${SPIKESTREAM_ROOT}\spikestream.config.template
AccessControl::GrantOnFile "$INSTDIR\spikestream.config" "(BU)" "FullAccess"

# SpikeStream Executable files
File /oname=bin\spikestream.exe ${SPIKESTREAM_ROOT}\bin\spikestream.exe
File /oname=bin\dbconfigtool.exe ${SPIKESTREAM_ROOT}\bin\dbconfigtool.exe

# Core SpikeStream library files
File /oname=bin\spikestream0.dll ${SPIKESTREAM_ROOT}\bin\spikestream0.dll
File /oname=bin\spikestreamapplication0.dll ${SPIKESTREAM_ROOT}\bin\spikestreamapplication0.dll

# SpikeStream Plugins
File /r /x .svn /x *.a /x *d4.dll ${SPIKESTREAM_ROOT}\plugins

# SpikeStream Patterns
File /r /x .svn ${SPIKESTREAM_ROOT}\*.pat

# Qt library files
File /oname=bin\QtCore4.dll ${QT_ROOT}\bin\QtCore4.dll
File /oname=bin\QtGui4.dll ${QT_ROOT}\bin\QtGui4.dll
File /oname=bin\QtOpenGL4.dll ${QT_ROOT}\bin\QtOpenGL4.dll
File /oname=bin\QtSql4.dll ${QT_ROOT}\bin\QtSql4.dll
File /oname=bin\QtXml4.dll ${QT_ROOT}\bin\QtXml4.dll
File /oname=bin\sqldrivers\qsqlmysql4.dll ${QT_ROOT}\plugins\sqldrivers\qsqlmysql4.dll

# MinGW libraries
File /oname=bin\libgcc_s_dw2-1.dll ${MINGW_ROOT}\bin\libgcc_s_dw2-1.dll
File /oname=bin\libstdc++-6.dll ${MINGW_ROOT}\bin\libstdc++-6.dll
File /oname=bin\mingwm10.dll ${MINGW_ROOT}\bin\mingwm10.dll

# NeMo libraries
File /oname=bin\nemo.dll ${SPIKESTREAM_ROOT}\bin\nemo.dll
File /oname=bin\nemo_base.dll ${SPIKESTREAM_ROOT}\bin\nemo_base.dll
File /oname=bin\nemo_cpu.dll ${SPIKESTREAM_ROOT}\bin\nemo_cpu.dll
File /oname=bin\nemo_cuda.dll ${SPIKESTREAM_ROOT}\bin\nemo_cuda.dll

# NeMo Plugins
SetOutPath $INSTDIR\bin
File /r ${SPIKESTREAM_ROOT}\bin\nemo-plugins
SetOutPath $INSTDIR

# iSpike and its dependencies
File /oname=bin\libiSpike.dll ${SPIKESTREAM_ROOT}\bin\libiSpike.dll
File /oname=bin\libboost_regex-mgw45-mt-1_46.dll ${SPIKESTREAM_ROOT}\bin\libboost_regex-mgw45-mt-1_46.dll
File /oname=bin\libboost_system-mgw45-mt-1_46.dll ${SPIKESTREAM_ROOT}\bin\libboost_system-mgw45-mt-1_46.dll
File /oname=bin\libboost_thread-mgw45-mt-1_46.dll ${SPIKESTREAM_ROOT}\bin\libboost_thread-mgw45-mt-1_46.dll

# iSpike test files
File /oname=bin\imageIn.ppm ${SPIKESTREAM_ROOT}\bin\imageIn.ppm
File /oname=bin\anglesIn.txt ${SPIKESTREAM_ROOT}\bin\anglesIn.txt
File /oname=bin\anglesOut.txt ${SPIKESTREAM_ROOT}\bin\anglesOut.txt

# Other library files
File /oname=bin\qwt5.dll ${SPIKESTREAM_ROOT}\bin\qwt5.dll
File /oname=bin\libmySQL.dll ${MYSQL_ROOT}\lib\libmySQL.dll
#File /oname=bin\vcomp90.dll ${SPIKESTREAM_ROOT}\bin\vcomp90.dll

# Images
File /r /x .svn /x test /x doc ${SPIKESTREAM_ROOT}\*.xpm
File /r /x .svn /x test /x doc ${SPIKESTREAM_ROOT}\*.png
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.ico

# Database files
File /r /x .svn /x test ${SPIKESTREAM_ROOT}\*.sql

# Document files
File /r /x .svn ${SPIKESTREAM_ROOT}\*.pdf
SetOutPath $INSTDIR\doc\api
File /r ${SPIKESTREAM_ROOT}\doc\api\application
File /r ${SPIKESTREAM_ROOT}\doc\api\applicationlibrary
File /r ${SPIKESTREAM_ROOT}\doc\api\library
File /r ${SPIKESTREAM_ROOT}\doc\api\nemo
SetOutPath $INSTDIR


