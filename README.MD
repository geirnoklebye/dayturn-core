



New Tools Update

================

Linux 64 Bit

------------



Development system:

-------------



Development system is Debian Stretch (testing). The compiler is gcc-4.9. If starting with a new Stretch you will need to install

    build tools; Cmake, Mercurial, Bison, Flex, Tortoisehg, Kdiff3 (or another desired merge tool) and Python27.

    Debian packages of the build tools work. Python27 is required so, after installing Python use 'python --version' to check the

    active version. Debian as with most linux distributions if difficult to make right on a hardware box but, it is easy to put in  a virtual machine.

  Library archives for the most part are built with gcc-4.6.4. Boost, Colladadom, and Googlemock are built with gcc-4.9.





Preprations to build:

--------------



Autobuild and Python:



- Clone Autobuild

    cd ~/

    hg clone https://bitbucket.org/lindenlab/autobuild or if your have ssh setup ssh://hg@bitbucket.org/lindenlab/autobuild

- Prepare Python

Install python-pip 

sudo apt-get install python-pip

Install python-dev  but, it might already be installed

sudo apt-get install python-dev

Pip Install autobuild python dependencies

sudo pip install 'hg+https://bitbucket.org/oz_linden/autobuild-metadata#egg=autobuild'



Clone, configure, and build Kokua-NT

----------------------



-  hg clone https://NickyP@bitbucket.org/NickyP/kokuant



  KokuaNT can be built with opensource or properity audio engine. The opensource solution uses openal for sounds and gstreamer for streaming music and audio visual files like mp4's. Use of the properity FMOD Ex library for sounds and streaming audio is supported but, the FMOD Ex library must be provided separately.



-  Configure for an openal and gstreamer build:

Following assumes a clean build tree.

cd kokuant

autobuild configure -c ReleaseOS -- -DLL_TESTS:BOOL=OFF -DFMODEX:BOOL=OFF -DOPENAL:BOOL=ON -DPACKAGE:BOOL=ON 2>&1 |tee configure.log



- Build the viewer

-autobuild build -c ReleaseOS 2>&1 |tee build.log



- Configuration and building takes about 1.5 hours on a 2 core machine.



- Test the build

cd build-linux-x86_64/newview/packaged

Install the viewer with

./install.sh select Y to place in user space.

This places the viewer in ~/.kokua-install and places a Kokua menu entry under Applications->Internet



Linux 32 Bit

------------



Development system is Debian Stretch (testing). The compiler is ~~gcc-4.6.4~~ gcc-4.9.2. If starting with a new Stretch you will need to install build tools;

    Cmake, Mercurial, Bison, Flex, Tortoisehg, Kdiff3 (or another desired merge tool) and Python27.

    Debian packages of the build tools work. Python27 is required so, after installing Python use 'python --version' to check the active version.

   	Debian as with most linux distributions if difficult to make right on a hardware box but, it is easy to put in  a virtual machine.

    Library archives for the most part are from lindenlab viewer-release. Some libraries have been rebuilt on gcc-4.6.4. Boost, Colladadom, Googlemock, are built on gcc-4.9.2
	
	to allow the viewer to be built. See the commits with [Linux 32 bit] tag in the message for a record of rebuilt libraries.





-  Configure for a Gstreamer/Openal build:

Following assumes a clean build tree.

cd kokuant

autobuild -v configure -c ReleaseOS -- -DLL_TESTS:BOOL=OFF -DFMODEX:BOOL=OFF -DOPENAL:BOOL=ON 2>&1 |tee configure.log



- Build the viewer

-autobuild build -c ReleaseOS 2>&1 |tee build.log



- Configuration and building takes about 1.5 hours on a 2 core machine.



- Test the build



./install.sh select Y to place in user space.

This places the viewer in ~/.kokua-install and places a Kokua menu entry under Applications->Internet



If you enconuter missing libalut.so at viewer start install the package libalut-dev. This well also install libalut0.



 For debain based sudo apt-get install libalut-dev.

 



Windows

--------

TBP



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







