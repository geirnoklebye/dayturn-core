 
#!/bin/bash
# Init
FILE="/tmp/out.$$"
GREP="/bin/grep"
#....
# Make sure only root can run our script
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root if you install regular user
   please use the ./install.sh command " 1>&2
   exit 1
fi

# ...

# Slackware build script for Kokua 
#
# This script will repackage the binary distribution into a Slackware package
#
# Martin Rogge <martin_rogge@users.sourceforge.net>

# Redistribution and use of this script, with or without modification, is
# permitted provided that the following conditions are met:
#
# 1. Redistributions of this script must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


# NOTE: this SlackBuild is made for the Opensim release of Kokua.
# It can be used for the Second Life main grid and beta grid as well.
# However, if you require those features specific to the Havoc release
# of Kokua you should build the standard release of Kokua
# for which a separate package is available.
# Both packages can be installed in parallel on the same installation.
# init
# Automatically determine the architecture we're building on:
MARCH=$( uname -m )
if [ -z "$SARCH" ]; then
  case "$MARCH" in
    i?86)    export ARCH=i486 ;;
    armv7hl) export ARCH=$MARCH ;;
    arm*)    export ARCH=arm ;;
    # Unless $ARCH is already set, use uname -m for all other archs:
    *)       export SARCH=$MARCH ;;
  esac
fi
# avoid overwriting the previous configuration

rm -rf /tmp/Drko
VTYPE="64"             ## uncommentment when building the viewer x86_64.
#VTYPE=""           #uncommentment when building the viewer 1686
#BUILDTYPE="X86"    # uncommentment when building the viewer 1686
BUILDTYPE="X86_64"    # uncommentment when building the viewer x86_64
#PKGBUILD="i686"         # uncommentment when building the viewer 1686 
PKGBUILD="X86_64"
PRGNAM=Kokua"$VTYPE"                           
VERSION=${VERSION:-_3.7.31.3746}      
BUILD=${BUILD:--2}
TAG=${TAG:-DRKO}
CWD=$(pwd)
TMP=${TMP:-/tmp/Drko}
CHAN=USL-Drakeo-$PKGBUILD
PKG=$TMP/package-$PRGNAM-$CHAN
OUTPUT=${OUTPUT:-/tmp/Drko}

VERSIONMV=kokua$VTYPE-USL-install
DESKTOP=kokua_$VTYPE-USL-viewer
BUILDNAME=Slackware-installer.sh
SHORTV=3.7.31              # edit for version change in indra/Version text 
mkdir -p $TMP/slin



SLACK_TYPE=`uname -m`
if [ ${SLACK_TYPE} == '$BUILDTYPE' ]; then 
  ARCH=$BUILDTYPE
  else
 ARCH=$BUILDTYPE

fi



function pause(){
   read -p "$*"
}
 
# ...
echo -e "\e[1;33m The installer has detected Your Systems is $MARCH   This a  $BUILDTYPE Program Kokua_$PKGBUILD\e[0m"
echo -e "\e[1;33m To use Kokua_i686   on x86_64  system you will need Alien Bob's Multi-lib. http://www.slackware.com/~alien/multilib/ \e[0m"
echo -e "\e[1;34m To use voice on a x86_64 with Kokua_86_64 you will need Alien Bob's Multi-lib. http://www.slackware.com/~alien/multilib/\e[0m"
echo -e "\e[1;34m Ok slacker going to create a new installpkg from the tarball.\e[0m"
echo -e "\e[1;33m Will Delete your old backup Create a new one. Before we upgrade.\e[0m"
echo -e "\e[1;32m first time installing press enter.\e[0m"
pause 'Press [Enter] key to continue or ctrl c to stop...'
# rest of the script
#


echo -e "\e[1;34m Removing old kokua$VTYPE-USL-install-backup.\e[0m"

sleep 3 
rm -rf /opt/kokua$VTYPE-USL-install.backup*

echo -e "\e[1;32m Backing up kokua$VTYPE-USL-install time and date.\e[0m"

sleep 3
/sbin/makepkg -l n -c n $TMP/slin/$PRGNAM-$CHAN$VERSION.tar.gz

mv $TMP/slin/$PRGNAM-$CHAN$VERSION.tar.gz  $CWD

cp -aR /opt/kokua"$VTYPE"-USL-install  /opt/kokua"$VTYPE"-USL-install.backup-$(date +%Y-%m-%d)


if [ -r  $PRGNAM-$CHAN$VERSION.tar.gz ]; then
 echo -e "\e[1;32m OK going to create the Slackware package.\e[0m"

fi

 
sleep 5 
 
rm -rf $TMP 


mkdir -p  $TMP/$PRGNAM-$CHAN$VERSION 

mkdir -p $TMP $PKG $OUTPUT
cd $TMP/$PRGNAM-$CHAN$VERSION 

tar xvf $CWD/$PRGNAM-$CHAN$VERSION.tar.gz


chown -R root:root .
find -L . \
 \( -perm 777 -o -perm 775 -o -perm 750 -o -perm 711 -o -perm 555 \
  -o -perm 511 \) -exec chmod 755 {} \; -o \
 \( -perm 666 -o -perm 664 -o -perm 640 -o -perm 600 -o -perm 444 \
 -o -perm 440 -o -perm 400 \) -exec chmod 644 {} \;

mkdir -p $PKG/opt/$VERSIONMV
# copy files just like the install script would do
cd $TMP/$PRGNAM-$CHAN$VERSION 
 cp -a * $PKG/opt/$VERSIONMV
# avoid overwriting the previous configuration
mkdir -p $PKG/usr/doc/kokua64-USL-$SHORTV


cd $TMP/$PRGNAM-$CHAN$VERSIONMV

cp -a README*.txt licenses.txt gpu_table.txt   $PKG/usr/doc/kokua64-USL-$SHORTV
cat $CWD/$BUILDNAME > $PKG/usr/doc/kokua64-USL-$SHORTV/$BUILDNAME
mkdir -p $PKG/install


cd $PKG

/sbin/makepkg -l y -c n $OUTPUT/$PRGNAM-$CHAN$VERSION-$TAG$BUILD.${PKGTYPE:-tgz}

echo -e "\e[1;32m The slackware installer package has been made.\e[0m"
echo -e "\e[1;32m it is located at /tmp/Drko.\e[0m"

# ...
function pause(){
   read -p "$*"
}

echo -e "\e[1;33m Would you like to install Kokua$VTYPE open sim edition if so.\e[0m"
pause 'Press [Enter] key to continue or ctrl c to stop...'
# rest of the script
#

upgradepkg --reinstall --install-new  $OUTPUT/$PRGNAM-$CHAN$VERSION-$TAG$BUILD.${PKGTYPE:-tgz}

sh /opt/kokua$VTYPE-USL-install/etc/refresh_desktop_app_entry.sh 

update-desktop-database
cd $CWD

rm -rf $PRGNAM-$CHAN$VERSION.tar.gz

rm -rf /tmp/Drko


