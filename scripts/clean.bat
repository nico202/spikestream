REM SpikeStream script that removes makefiles and any build files, objects and libraries

REM Run make clean at root - if SpikeStream has been built from here, this should clean everything
cd ..
mingw32-make distclean

REM Clean up files at root
rm Makefile

REM Clean up liveliness files
cd analysis/liveliness
mingw32-make distclean
rm liveliness.pro.user
rm liveliness_resource.rc

REM Clean up state based phi files
cd ../statebasedphi
mingw32-make distclean
rm statebasedphi.pro.user
rm statebasedphi_resource.rc

REM Clean up application files
cd ../../application
mingw32-make distclean
rm application.pro.user
rm spikestream_resource.rc
rm -R debug
rm -R release

REM Clean up application library


 
