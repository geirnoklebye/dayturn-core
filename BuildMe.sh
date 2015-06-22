#!/bin/bash

NPROC=$(nproc)
export AUTOBUILD_PLATFORM_OVERRIDE='linux64'

autobuild  configure -c ReleaseOS -- -DLL_TESTS:BOOL=OFF -USESYSTEMLIBS:BOOL=ON -DWORD_SIZE=64  -DFMODEX:BOOL=ON  -DFMODEX_SDK_DIR=/opt/fmodapi44454linux/  -DOPENAL:BOOL=ON -DPACKAGE:BOOL=ON 2>&1 |tee configure.log

cd build-linux-x86_64  

make -j$NPROC

#cmake -G Ninja
