#!/bin/bash

# ----------------------------------------------------------
# Fixes the library path after macdeployqt has been run.
# Run this after deploynemo.sh
# ----------------------------------------------------------

# Special fixes
install_name_tool -change libnemo.dylib @executable_path/../Frameworks/libnemo.dylib ../../plugins/simulation/libnemowrapper.0.2.0.dylib


# Fix iSpike library in wrapper
install_name_tool -id @executable_path/../Frameworks/libiSpike.dylib  ../../bin/spikestream.app/Contents/Frameworks/libiSpike.dylib
install_name_tool -change libiSpike.2.1.dylib @executable_path/../Frameworks/libiSpike.dylib ../../plugins/simulation/nemodevices/libispike.0.2.0.dylib
install_name_tool -change libnemo.dylib @executable_path/../Frameworks/libnemo.dylib ../../plugins/simulation/nemodevices/libispike.0.2.0.dylib


# Sort out iSpike Boost libraries
install_name_tool -id @executable_path/../Frameworks/libboost_regex-mt.dylib ../../bin/spikestream.app/Contents/Frameworks/libboost_regex-mt.dylib 
install_name_tool -id @executable_path/../Frameworks/libboost_system-mt.dylib../../bin/spikestream.app/Contents/Frameworks/libboost_system-mt.dylib 
install_name_tool -id @executable_path/../Frameworks/libboost_thread-mt.dylib ../../bin/spikestream.app/Contents/Frameworks/libboost_thread-mt.dylib

install_name_tool -change /opt/local/lib/libboost_regex-mt.dylib @executable_path/../Frameworks/libboost_regex-mt.dylib ../../bin/spikestream.app/Contents/Frameworks/libiSpike.dylib 
install_name_tool -change /opt/local/lib/libboost_system-mt.dylib @executable_path/../Frameworks/libboost_system-mt.dylib ../../bin/spikestream.app/Contents/Frameworks/libiSpike.dylib 
install_name_tool -change /opt/local/lib/libboost_thread-mt.dylib @executable_path/../Frameworks/libboost_thread-mt.dylib ../../bin/spikestream.app/Contents/Frameworks/libiSpike.dylib 


# Change the rest of the plugins
for file in neurons/libcuboid connections/librandom1 connections/libtopographic1 simulation/libnemowrapper analysis/libliveliness analysis/libstatebasedphi \
			networks/libalekgameznets2 networks/libaleksandernetworks networks/libconnectionmatriximporter networks/libnrmimporter networks/libtononinetworks \
			networks/libizhikevichetworks simulation/nemodevices/libispike
do

# Remove files to prevent duplicate versions of plugin being loaded
rm ../../plugins/$file.0.2.dylib
rm ../../plugins/$file.0.dylib
rm ../../plugins/$file.dylib

#Change paths
echo "Changing paths in " $file.0.2.0.dylib
install_name_tool -change libqwt.5.dylib @executable_path/../Frameworks/libqwt.5.dylib ../../plugins/$file.0.2.0.dylib
install_name_tool -change /opt/local/lib/libgmpxx.4.dylib @executable_path/../Frameworks/libgmpxx.4.dylib ../../plugins/$file.0.2.0.dylib
install_name_tool -change /opt/local/lib/libgmp.3.dylib @executable_path/../Frameworks/libgmp.3.dylib ../../plugins/$file.0.2.0.dylib
install_name_tool -change libspikestreamapplication.0.dylib @executable_path/../Frameworks/libspikestreamapplication.0.dylib ../../plugins/$file.0.2.0.dylib
install_name_tool -change libspikestream.0.dylib @executable_path/../Frameworks/libspikestream.0.dylib ../../plugins/$file.0.2.0.dylib
install_name_tool -change QtSql.framework/Versions/4/QtSql @executable_path/../Frameworks/QtSql.framework/Versions/4/QtSql ../../plugins/$file.0.2.0.dylib
install_name_tool -change QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml.framework/Versions/4/QtXml ../../plugins/$file.0.2.0.dylib
install_name_tool -change QtOpenGL.framework/Versions/4/QtOpenGL @executable_path/../Frameworks/QtOpenGL.framework/Versions/4/QtOpenGL ../../plugins/$file.0.2.0.dylib
install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui ../../plugins/$file.0.2.0.dylib
install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore ../../plugins/$file.0.2.0.dylib

done

