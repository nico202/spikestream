#!/bin/bash

echo "Fixing deployed NeMo library IDs and paths"

# Fix the IDs of the deployed NeMo libraries - Gives the file specified in the second argument the name specified in the first argument
install_name_tool -id @executable_path/../Frameworks/libnemo.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo.dylib
install_name_tool -id @executable_path/../Frameworks/libnemo_base.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo_base.dylib
install_name_tool -id @executable_path/../Frameworks/libnemo_cpu.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo_cpu.dylib

# Fix the internal paths between the deployed NeMo libraries - The file specified in the third argument has a dependent library's name changed from the first argument to the second
install_name_tool -change libnemo_base.dylib @executable_path/../Frameworks/libnemo_base.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo.dylib
install_name_tool -change libnemo_cpu.dylib @executable_path/../Frameworks/libnemo_cpu.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo.dylib
install_name_tool -change libnemo_base.dylib @executable_path/../Frameworks/libnemo_base.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo_cpu.dylib

# Fix the dependency on libltdl
install_name_tool -id @executable_path/../Frameworks/libltdl.dylib ../../bin/spikestream.app/Contents/Frameworks/libltdl.7.dylib

install_name_tool -change /usr/lib/libltdl.7.dylib @executable_path/../Frameworks/libltdl.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo.dylib
install_name_tool -change /usr/lib/libltdl.7.dylib @executable_path/../Frameworks/libltdl.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo_base.dylib
install_name_tool -change /usr/lib/libltdl.7.dylib @executable_path/../Frameworks/libltdl.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo_cpu.dylib

# Fix the NeMo plugin libraries, which depend on nemo_base
install_name_tool -change libnemo_base.dylib @executable_path/../Frameworks/libnemo_base.dylib ../../bin/nemo-plugins/cpu/libIF_curr_exp.dylib
install_name_tool -change /usr/lib/libltdl.7.dylib @executable_path/../Frameworks/libltdl.dylib ../../bin/nemo-plugins/cpu/libIF_curr_exp.dylib

install_name_tool -change libnemo_base.dylib @executable_path/../Frameworks/libnemo_base.dylib ../../bin/nemo-plugins/cpu/libInput.dylib
install_name_tool -change /usr/lib/libltdl.7.dylib @executable_path/../Frameworks/libltdl.dylib ../../bin/nemo-plugins/cpu/libInput.dylib

install_name_tool -change libnemo_base.dylib @executable_path/../Frameworks/libnemo_base.dylib ../../bin/nemo-plugins/cpu/libIzhikevich.dylib
install_name_tool -change /usr/lib/libltdl.7.dylib @executable_path/../Frameworks/libltdl.dylib ../../bin/nemo-plugins/cpu/libIzhikevich.dylib

install_name_tool -change libnemo_base.dylib @executable_path/../Frameworks/libnemo_base.dylib ../../bin/nemo-plugins/cpu/libKuramoto.dylib
install_name_tool -change /usr/lib/libltdl.7.dylib @executable_path/../Frameworks/libltdl.dylib ../../bin/nemo-plugins/cpu/libKuramoto.dylib

install_name_tool -change libnemo_base.dylib @executable_path/../Frameworks/libnemo_base.dylib ../../bin/nemo-plugins/cpu/libPoissonSource.dylib
install_name_tool -change /usr/lib/libltdl.7.dylib @executable_path/../Frameworks/libltdl.dylib ../../bin/nemo-plugins/cpu/libPoissonSource.dylib
				
