#!/bin/bash

echo "Fixing deployed NeMo library IDs and paths"

# Fix the IDs of the deployed NeMo libraries
install_name_tool -id @executable_path/../Frameworks/libnemo.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo.dylib
install_name_tool -id @executable_path/../Frameworks/libnemo_base.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo_base.dylib
install_name_tool -id @executable_path/../Frameworks/libnemo_cpu.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo_cpu.dylib

# Fix the internal paths between the deployed NeMo libraries
install_name_tool -change libnemo_base.dylib @executable_path/../Frameworks/libnemo_base.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo.dylib
install_name_tool -change libnemo_cpu.dylib @executable_path/../Frameworks/libnemo_cpu.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo.dylib
install_name_tool -change libnemo_base.dylib @executable_path/../Frameworks/libnemo_base.dylib ../../bin/spikestream.app/Contents/Frameworks/libnemo_cpu.dylib

