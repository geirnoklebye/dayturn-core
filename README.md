BUILD USESYSTEMLIBS SLAKWARE 14.1 HOW TO READ BELOW.
This is my custom Firestorm I have been building for a few years. It has openjpeg 1.5.2 built 
into the build like singularity.  This repo will stay up to date with Kokua 

This build is for slackware64 14.1 should build in slackware64 14.0  Slackware64 14.1+current 
you will need it to edit the /indra/cmake/PNG.cmake to libpng16. I would wait I have a new build script for current
coming out very soon.
The media plugins I have made easy to drop in require libpng14 so you may need to copy your own 
webkit dynamics over for current.for voice you will need. 
Alien Bobs multilib . http://www.slackware.com/~alien/multilib/ below I have a install script for that.

If you want my scripts that maintain your system multilib and can make distro upgrading easy go here.
http://www.linuxquestions.org/questions/slackware-14/multilib-question-4175515564/#post5224104

If you are building with multilib you will need to keep the custom 3p-builds from trying to link to usr/lib because we are building
/usr/lib64 64 bit stuff. all the Viewer specific go in /usr/local/include and /usr/local/lib64 this make it simple for distro upgrades and updates. 
So during the build rename /usr/lib to /usr/lib~ when done building and installing you will see an empty folder /usr/lib
that is left over from the build you may delete that. ######!!!Remember To Change /usr/lib~ back to /usr/lib !!!!######## 
after you are done building  you may like to know Slackware can build 32 and 64 bit builds on the same machine.
 !!!!!!!!!!! NOT USESYSYTEMLIBS !!!!!!!!! That is only for 64bit. 

you will see two files "1-get-libs and 2-build-libs" This will upgrade your libboost to 1.57 and it also builds shared and static.
and we needed a shared and static build of hunspell.
take those two files "1-get-libs and 2-build-libs" and create a directory put them in it.
 run the first one sh 1-get-libs and the second one must be run as root user. sh 2-build-libs
 This build will not affect your Slackware build ##!EXCEPT!## for the custom gstreamer-0.10 header files in /usr/include.
 The originals are backed up and dated. you may switch them back at anytime or slackpkg reinstall gstreamer. 
 The boost headers are LL and Firestorm custom tweaked headers. I have been using them for a few years no problems to my system.
  
The build script builds many things it takes them from the official SlackBuilds.org Repo's
and from NickyD and  tonyasouther  bitbucket. and my modified forks of them. all of the SlackBuild stuff can
stay if you want to uninstall it. The last package we build has all the Systemlibs we need to build and run the viewer.
and all the others are their USL dependency's for everything.  

 
These will build and install everything you need to build  Firestorm USL USESYSYTEMLIBS.

when done just reboot your computer. cd to firestorm and run the command below.

autobuild -m64 configure  -c ReleaseFS_open -- --chan USL-BUILD  -USESYSTEMLIBS:BOOL=ON    -DFMODSTUDIO:BOOL=ON  -DFMODSTUDIO_SDK_DIR=/opt/firestorm/fmodstudioapi10601linux -DNDOF:BOOL=ON

this will create a folder "build-linux-x86_64" CD "change directories to it. and do a make -jX <--- change the X to the number of cpu cores you have. I have a AMD 8350 8 cores
 so I stack my jobs a 8 at a time "-j8"
 
 This will happen half way through the build. error do not worry.

 Installed packages file '/media/63a10037-fcdb-4cab-851c-f6783cb383c0/Z-build/bukdfirestorm/firestorm32/NewFolder/nelib/phoenix-firestorm-lgpl/build-linux-x86_64/packages/installed-packages.xml' not found; creating.
Unrecognized --versions output: 

If it does just type make -jX and it will now go onto make2 setup. and it will finish the build and pack it. 

 Now after it is built in build-linux-x86_64/newview/packaged/
 you can drop my dictionary voice and media plugins in how. cd inside build-linux-x86_64/newview/packaged/ open a terminal and type 
 "wget https://bitbucket.org/Drakeo/firestorm-testing-boost/downloads/diction-voice-media-codecs-webkit.tar.bz2" it will download the tar ball now run
 tar xvf diction-voice-media-codecs-webkit.tar.bz2  this will put everything in the right place. now you can delete the tarball.
 Remember Firestorm has a hidden 1.4 gig file name ".debug" in the packaged/bin folder. you can delete it. you can delete the built package also
 and re run make -jX and it will rebuild a package for you to put on your other Slackware machines. 
 have fun this has been tested on fresh installs of Slackware and heavily modified ones also.
 
alieb bob multilib install script. 
create a file name it "multilib-install" with out the quotes. past this in it.


 #!/bin/bash
# Init
FILE="/tmp/out.$$"
GREP="/bin/grep"
#....
# Make sure only root can run our script
if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root " 1>&2
   exit 1
fi

# ...
##UPGRADE MULTI-LIB  ######
rsync -r -a -v --bwlimit=500 --delete --progress --stats rsync://rsync.slackware.org.uk/people/alien/multilib/14.1/ /var/cache/multilib/
cd /var/cache/multilib
upgradepkg --reinstall --install-new *.t?z
cd /var/cache/multilib/slackware64-compat32
upgradepkg  --install-new *?/*.t?z

you can use this script to keep your 32bit and multi-arch gcc up to date
just run it after your slackpkg upgrade-all.
remember to blacklist the alien and  slackbuild packages, how.
edit your /etc/slackpkg/blacklist from this
#[0-9]+_SBo
#[0-9]+alien
#[0-9]+compat32

To this 

[0-9]+_SBo
[0-9]+alien
[0-9]+compat32

######### !!!!!!!!!!!!!!!!!!!!!!############## REMEMBER No space to the left of them or it will not take effect ##############