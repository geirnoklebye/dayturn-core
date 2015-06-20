#!/bin/bash
CWD=$(pwd)
set -e
cd build-linux-x86_64/newview/packaged
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
pause 'Press [Enter] key to continue or ctrl c to stop...'
# rest of the script
#
rm -rf diction-voice-media-codecs-webkit.tar.bz2
./install.sh



