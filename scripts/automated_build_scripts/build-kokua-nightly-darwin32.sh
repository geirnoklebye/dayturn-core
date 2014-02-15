#!/bin/bash
#export AUTOBUILD_PLATFORM_OVERRIDE='linux64'
TOP="/Users/nicky"
##
#rm -Rf "$TOP/kokua/build-darwin-i386"
SOURCE_DIR="$TOP/kokua/indra/newview"
BUILD_DIR="$TOP/kokua/build-darwin-i386"
stage="$BUILD_DIR/newview/"
pushd "$TOP/kokua"
rm -v -f ~/NightBuildResult/Darwin.log
date  2>&1 |tee Darwin.log
hg pull -u  2>&1 |tee -a Darwin.log
~/autobuild/bin/autobuild configure -v -c ReleaseOS -- -DLL_TESTS=OFF -DFMODEX=OFF -DOPENAL=ON -DPACKAGE=ON -DQUICKTIME=ON -DVIEWER_CHANNEL="\"Kokua Test\""
#check for errors
~/autobuild/bin/autobuild build -c ReleaseOS --no-configure -- -j 3 2>&1 |tee -a Darwin.log
date 2>&1 |tee -a Darwin.log
#check for errors
cp -v Darwin*.log ~/NightBuildResult/ 2>&1 |tee -a Darwin.log
popd 
pushd "$BUILD_DIR/newview"
cp -v *.dmg ~/NightBuildResult/ 2>&1 |tee -a Darwin.log
popd
rsync -avP -e ssh ~/NightBuildResult/*.dmg ~/NightBuildResult/Darwin.log nickyp@frs.sourceforge.net:/home/frs/project/kokua.team-purple.p/Nightly/
#sudo shutdown -h now



