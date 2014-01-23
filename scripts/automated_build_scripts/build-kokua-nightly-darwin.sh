#!/bin/bash
#export AUTOBUILD_PLATFORM_OVERRIDE='linux64'
TOP="/home/bill"
rm -Rf kokua
hg clone https://bitbucket.org/kokua/kokua-beta kokua
#hg clone http://192.168.1.4:8000 kokua
SOURCE_DIR="$TOP/kokua/indra/newview"
BUILD_DIR="$TOP/kokua/build-linux-i686"
pushd "$TOP/kokua"
rm -v -f ~/Release/Linux32.log
date  2>&1 |tee Linux32.log
#hg fetch 2>&1 |tee -a Linux32.log
$TOP/autobuild/bin/autobuild configure -c ReleaseOS -- -DLL_TESTS=OFF -DPACKAGE=ON -DVIEWER_CHANNEL="\"Kokua Release\"" 2>&1 |tee -a Linux32.log
#check for errors
$TOP/autobuild/bin/autobuild build -c ReleaseOS 2>&1 |tee -a Linux32.log
date 2>&1 |tee -a Linux32.log
#check for errors
cp -u -v Linux32*.log ~/Release/ 2>&1 |tee -a Linux32.log
popd 
pushd "$BUILD_DIR/newview"
cp -u -v *.tar.bz2 ~/Release/
popd



