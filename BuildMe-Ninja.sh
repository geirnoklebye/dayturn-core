#!/bin/bash
CWD=$(pwd)
NPROC=$(nproc)
#export AUTOBUILD_PLATFORM_OVERRIDE='linux64'

autobuild  configure -c ReleaseOS -- cmake -G Ninja -DLL_TESTS:BOOL=OFF -USESYSTEMLIBS:BOOL=ON -DWORD_SIZE=64  \
-DFMODEX:BOOL=ON  -DFMODEX_SDK_DIR=/opt/fmodapi44454linux/  -DOPENAL:BOOL=ON \
-DPACKAGE:BOOL=ON --platform linux64 2>&1 |tee configure.log

cp $CWD/Slackware/installed-packages.xml  build-linux-i686/packages 

cd $CWD/build-linux-i686  

ninja -j$NPROC



set -e
# ...
function pause(){
   read -p "$*"
}

echo -e "\e[1;33m Would you like to install the dictionary for spell check
            plus Slvoice and the plugins you can also do that manually if you like.\e[0m"
pause 'Press [Enter] key to install them or ctrl c to stop...'
# rest of the script
#
cd $CWD/build-linux-i686/newview/packaged
echo -e "\e[1;34m Downloading SLvoice, dictionary .\e[0m"
sleep 3 
wget -c https://bitbucket.org/Drakeo/firestorm-testing-boost/downloads/diction-voice-media-codecs-webkit.tar.bz2
tar xvf diction-voice-media-codecs-webkit.tar.bz2
echo -e "\e[1;34m installed them now going to link your system qtwebkit.\e[0m"

# ...
function pause(){
   read -p "$*"
}

echo -e "\e[1;33m If you have not seen any errors Would you like to install the viewer now .\e[0m"
pause 'Press [Enter] key to build a slackware pkg or ctrl c to stop and just install with linden labs installer...'
# rest of the script
#
rm -rf diction-voice-media-codecs-webkit.tar.bz2

cp $CWD/Slackware/Slackware-installer.sh  $CWD/build-linux-i686/newview/packaged

sh Slackware-installer.sh 



#cmake -G Ninja
