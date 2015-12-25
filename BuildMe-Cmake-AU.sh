#!/bin/bash
CWD=$(pwd)
NPROC=$(nproc)
export AUTOBUILD_PLATFORM_OVERRIDE='linux64'
autobuild  configure -c ReleaseOS -- -DFMODEX:BOOL=ON -DLL_TESTS:BOOL=OFF  \
   -DOPENAL:BOOL=ON \
-DPACKAGE:BOOL=ON --platform linux64 2>&1 |tee configure.log
ln -s build-linux-x86_64  build-linux-i686


cd $CWD/build-linux-x86_64 

make -j$NPROC



