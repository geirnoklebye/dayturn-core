#!/bin/bash
#export AUTOBUILD_PLATFORM_OVERRIDE='linux64'
TOP="/Users/nickyperian"
## Following adapted from onefang Rejected's Imprudence build sctipts
# Try to figure out which SDK is the earliest this version of Xcode
# supports.  This might be fragile, only tested it with Xcode 3.2.6,
# which is the minimum supported Xcode anyway.
sdk_path="$(xcodebuild -version -sdk | grep Path: | cut -d ' ' -f 2 | \
grep MacOSX10. | sort | head -n 1)"
# Save both parts, in case there's stuff in the path that the magic
# foo below might get confused over.
sdk_file="$(basename $sdk_path)"
sdk_path="$(dirname $sdk_path)"
sdk="$(echo $sdk_file | cut -d '.' -f 2)"

# Minimum of SDK 10.5
if [ "$sdk" == "4u" ] ; then sdk_file="MacOSX10.5.sdk"; fi

defs_extra="-DCMAKE_OSX_SYSROOT=$sdk_path/$sdk_file"
##
#rm -Rf "$TOP/kokua/build-darwin-i386"
SOURCE_DIR="$TOP/kokua/indra/newview"
BUILD_DIR="$TOP/kokua/build-darwin-i386"
stage="$BUILD_DIR/newview/"
pushd "$TOP/kokua"
rm -v -f ~/NightBuildResult/Darwin.log
date  2>&1 |tee Darwin.log
hg pull -u  2>&1 |tee -a Darwin.log
~/autobuild/bin/autobuild configure -v -c ReleaseOS -- $defs_extra -DLL_TESTS=OFF -DFMODEX=OFF -DOPENAL=ON -DPACKAGE=ON -DQUICKTIME=ON -DVIEWER_CHANNEL="\"Kokua Test\""
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



