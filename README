



New Tools Update

================

Linux 64 Bit

------------



Development system:

-------------



Development system: Ubuntu 14.04.3 

3.19.0-47-generic #53~14.04.1-Ubuntu SMP Mon Jan 18 16:09:14 UTC 2016 x86_64 x86_64 x86_64 GNU/Linux


Preprations to build:

--------------

sudo apt-get update

sudo apt-get upgrade

sudo apt-get install --install-recommends bison bzip2 cmake curl flex g++-4.8 m4 mercurial python2.7 python2.7-dev python-pip

sudo apt-get install --install-recommends libgl1-mesa-dev libglu1-mesa-dev libstdc++6 \
libX11-dev libxinerama-dev libxml2-dev libxrender-dev libpulse-dev libalut-dev 

Verify
gcc --version
gcc (Ubuntu 4.8.4-2ubuntu1~14.04) 4.8.4

Install autobuild into python

sudo pip install hg+https://bitbucket.org/lindenlab/autobuild-1.0#egg=autobuild

Install optional tools

sudo apt-get install --install-recommends tortoisehg kdiff3 mc

Optionally install gcc-version 4.6 which is needed to build library archives,

sudo apt-get install --install-recommends gcc-4.6 g++-4.6 cpp-4.6

If using ssh

mkdir ~/.ssh

copy you keys to this directory

cd ~/.ssh

sudo chmod 600 id_rsa

cd ~/

sudo chmod 700 .ssh

Voice libraries are already installed on 32 bit systems so, the steps can be skipped.

Voice 32 bit libraries are not needed to build the viewer but, are needed to test voice in the viewer.

sudo dpkg --add-architecture i386

sudo apt-get update

sudo apt-get install --install-recommends libasound2:i386 libasound2-plugins:i386 libasyncns0:i386 libattr1:i386 libc6:i386 libc6-i686:i386 libcap2:i386 libdbus-1-3:i386 libflac8:i386 libgcc1:i386 libice6:i386 libidn11:i386 libjson0:i386 libogg0:i386 libpulse0:i386 libsm6:i386 libsndfile1:i386 libstdc++6:i386 libvorbis0a:i386 libvorbisenc2:i386 libwrap0:i386 libuuid1:i386 libx11-6:i386 libx11-xcb1:i386 libxau6:i386 libxcb1:i386 libxdmcp6:i386 libxext6:i386 libxi6:i386 libxtst6:i386 zlib1g:i386 

 
Following applies to building 32 bit kokua by using a 32 bit virtual machine(vm). If building on 32 bit hardware -- follow 64 bit instructions from above.

Install a 32 bit vm from iso -- as of January 20, 2016 
ubuntu-14.04.3-desktop-i386.iso

Open your vm and follow instructions for 64 bit from above. 

Below is a sample ~/.hgrc (mercurial.ini) file. This uses tortoisehg or command line mercurial, kdiff3 as a merge tool and gedit as a visual editor.
The visual editor may be changed based on personal perference.


[tortoisehg]

defaultwidget = mq

opentabsaftercurrent = True

authorcolor = True

longsummary = True

[extensions]

hgext.bookmarks =

hgext.extdiff =

hgext.convert =

color =

fetch =

hgext.mq=

transplant=

eol =


[extdiff]

cmd.kdiff3 = 

[merge-tools]

kdiff3.priority=1

kdiff3.args=--L1 base --L2 local --L3 other $base $local $other -o $output

kdiff3.fixeol=True

kdiff3.gui=True

[tortoisehg]

authorcolor = True

vdiff = kdiff3

editor = gedit

[diff-tools]

kdiff3.diffargs=--L1 '$plabel1' --L2 '$clabel' $parent $child

[ui]

username = Nicky Perian <nickyperian@yahoo.com

ssh = ssh -C

[patch]

eol = auto

[hooks]

#pretxncommit = python:~/hg-tools/coding_policy.py:hook

#pretxnchangegroup = python:~/hg-tools/coding_policy.py:hook

----------------------
As an options add this to you bash history file ~/.bashrc

Otherwise enter into the terminal before autobuild configure step

Skip the export for 32 bit builds.

export AUTOBUILD_PLATFORM_OVERRIDE='linux64'


-  hg clone https://NickyP@bitbucket.org/NickyP/kokuant

  KokuaNT can be built with opensource or properity audio engine. The opensource solution uses openal for sounds and gstreamer for streaming music and audio visual files like mp4's. Use of the properity FMOD Ex library for sounds and streaming audio is supported but, the FMOD Ex library must be provided separately.

-  Configure for an openal and gstreamer build:

Following assumes a clean build tree.

cd kokuant

Update the source tree to KokuaNT. This is a build without RLV or if you want RLV it would be hg update Kokua-MKRLV

hg update KokuaNT


autobuild configure -c ReleaseOS -- -DLL_TESTS:BOOL=OFF -DFMODEX:BOOL=OFF -DOPENAL:BOOL=ON -DPACKAGE:BOOL=ON 2>&1 |tee configure.log

- Build the viewer

-autobuild build -c ReleaseOS 2>&1 |tee build.log

- Configuration and building takes about 1.5 hours on a 2 core machine.

- Test the build

cd build-linux-x86_64/newview/packaged

Install the viewer with

sudo ./install.sh follow the defaults

This places the viewer in /opt/kokua-install and places a Kokua menu entry under Applications->Internet

sudo is the perferred method as the chrome-sandbox a part of Chrome Embedded Framework requies root permissions.


Windows

To be published

--------

Mac

--------

Current development system is OS X 10.10.4 with Xcode 6.4 and the latest Mac version of Mercurial for source management. 

The viewer is built with the OS X 10.9 SDK with a deployment target of 10.9.

There currently exist some dependencies in the code base to the old Carbon frameworks (specifically Text Edit) that cannot be compiled 64-bit, so we are going to build a 32-bit version meaning all dependencies should also be built 32-bit. 

Performance-wise a 32-bit build should be more or less identical to a 64-bit version as the biggest performance factor lies in the renderer which would have to be rewritten to something else to gain significant performance increase (METAL). 

The 32-bit viewer can address and use a full 4GB of system memory which is more than sufficient unless you are in very large OpenSim VAR regions. In addition it will use the memory installed on the GPU for texture cacheing.

	-- 

Linden’s version of autobuild requires a different version of Python than the system installed so the best way to get it installed is to first install MacPorts from https://www.macports.org with the latest current release (2.3.3).

With MacPorts installed, in terminal install the following ports:

• sudo port install python27

• sudo port install py27-pip

• sudo port install cmake

When prompted by the installer run python_select to use the version you just installed. It will be installed in /opt/local/bin

If you have newer versions of Xcode installed then you also need to run xcode-select to make sure you use 6.4 for your build

--

Xcode should have created the directory ~/Library/Developer during installation. If not create it (or use the location of your choice) and shortcut it to the Finder sidebar. 

In terminal cd to the above directory and type the command:

hg clone https://bitbucket.org/lindenlab/autobuild

Pip Install auto build python dependencies by typing:

sudo pip install ‘hg+https://bitbucket.org/oz_linden/autobuild-metadata#egg=autobuild'

If everything goes well it should be installed in /opt/local/Library/Frameworks/Python.framework/Versions/2.7/bin/autobuild

To make life easier edit your .bash_profile and add the lines

alias autobuild="/opt/local/Library/Frameworks/Python.framework/Versions/2.7/bin/autobuild"

export AUTOBUILD=/opt/local/Library/Frameworks/Python.framework/Versions/2.7/bin/autobuild

Then source your .bash_profile

--

To verify your build environment the best way forward is most likely to download the LL source by following the instructions on http://wiki.secondlife.com/wiki/Compiling_the_viewer_(Mac_OS_X_XCode_6.1)

You should be able to both use the Xcode project (easiest to verify) and the command line build. 

NOTE: Regardless of which configuration option you use on the command line the Xcode project will have the build mode set to Debug. To change this go to Product > Scheme > Edit Scheme (with ALL_BUILD selected) and change the Build Configuration to RelWithDebInfo or Release respectively

BUILD NOTE: When building in Xcode at some point the build will fail because it cannot find packages-info.txt. At this point just restart the build and it will continue from there.

The root cause of this is that it tries to run autobuild by spawning a shell from inside autobuild, but Xcode will not allow any other version than the system python to be called so autobuild will fail - it does not even find it.  For anything but a (final) release build this is not significant. This build has to be done from the command line. 

--

To build Kokua first download the Kokua source code with the following command in terminal:

hg clone https://NickyP@bitbucket.org/NickyP/kokuant 

You can configure the build with:

autobuild configure -c RelWithDebInfoOS — -DKDU:BOOL=FALSE -DFMODEX:BOOL=TRUE -DLL_TESTS:BOOL=FALSE -DOPENAL:BOOL=FALSE

or 

autobuild configure -c ReleaseOS — -DKDU:BOOL=FALSE -DFMODEX:BOOL=TRUE -DLL_TESTS:BOOL=FALSE -DOPENAL:BOOL=FALSE

When you have made sure your configuration is working (and compiles in Xcode) you can also compile on the command line by substituting configure with build in the two commands above. 

Disclaimer

-----

-      This software is not provided nor supported by Linden Lab, the makers of Second Life.







