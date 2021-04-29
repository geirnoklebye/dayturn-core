Updated 17 April 2021

================
Please note: We do not provide support for self-building outside of the active Kokua team. This is due in part to the use of licenced components which we cannot share in buildable form and also the limited resources of the current team. Before trying to build Kokua make sure that you can successfully build the standard LL Viewer. At this writing several repositories no longerp rovide Python2 updates. To update to the last supplied version 2.7.18 folllow the procdure at:

https://tecadmin.net/install-python-2-7-on-ubuntu-and-linuxmint/

--------------------------

Contributing Code to Kokua

Contributing one patch with less than 3 files touched can be submitted as a diff or patch file. Please open a jira issue and attach the file to the issue.

Our issue tracker is at https://kokua.atlassian.net/secure/Dashboard.jspa

Contributions by frequent contributors or Kokua Team members requires a formalized work flow as outlined below.

Kokua repositories use git branches. We do not specify a master branch because the Kokua repo maintains three different releases so there is no single "master". Kokua-MKRLV is the closest we have to a master branch and if you clone kokua-release repository it will default to Kokua-MKRLV as that is the most popular Kokua variant (and also usually the most recently committed to since it gets updated in the last stage of the work flow below).

Work Flow:

On bitbucket fork kokua-release to your Bitbucket account. The repository name will default to kokua-release but can be named as desired.

Now is when you should clone your fork to your local file system. 

We use SmartGit for graphical display of our repositories. Open the repository and notice branches named Kokua-MKRLV and Kokua-NORLV; these represent our most used branches.

Our workflow starts with lindenlab/viewer master branch which is assigned branch LL-VIEWER-RELEASE-TIP within kokua-release repository. At this point we should be able

Next, LL-VIEWER-RELEASE-TIP is merged into Kokua-NORLV. This binds the latest SL default viewer to Kokua-NORLV which becomes a kokua deliverable. The merge is usually taken from one commit behind the LL tip to avoid including the version number increment which LL perform after promoting a release.

Then, Kokua-NORLV is merged into Kokua-MKRLV. Two deliverables, Kokua-MKRLV and Kokua-FTRLV may result from building the Kokua-MKRLV branch. 

When contributing a change it should be implemented first on Kokua-NORLV and then merged to Kokua-MRKLV. There are two exceptions to this. The first is when the change is only for the with-RLV versions of Kokua - make the change on the Kokua-MKRLV branch. The other is when it's likely the commit will be modified after being accepted - in this case it's simpler to only commit on NORLV and leave us to do the merge to MKRLV once the NORLV code is approved. 


When LindenLab updated to autobuild 1.1 most custom build variables were moved to a repository. The kokua custom repository should be cloned as follows:

git clone https://bitbucket.org/kokua/viewer-build-variables.git

The variables file in this repository contains custom build variables for windows macOS and linux.

An environment variable must be set in order for autobuild to see custom build variables throughout configure and build phases.

Follow each operating systems procedure for persistent setting of environment variables. 

For linux it is:

export AUTOBUILD_VARIABLES_FILE=/home/<user>/viewer-build-variables/variables

If not already done, use git clone to place your forked kokua repository on your local file system. Example: 

Example:

git clone https://bitbucket.org/kokua/kokua-release.git



Linux 64 Bit

------------



Development system:

-------------



Development system: Ubuntu 16.04.1 

4.15.0-34-generic #37~16.04.1-Ubuntu SMP Tue Aug 28 10:44:06 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux


Preparations to build:

--------------

sudo apt-get update

sudo apt-get upgrade

sudo apt-get install --install-recommends bison bzip2 cmake curl flex g++-5.4,m4 mercurial python2.7 python2.7-dev python-pip

sudo apt-get install --install-recommends pulseaudio

sudo apt-get install --install-recommends libgl1-mesa-dev libglu1-mesa-dev libstdc++6 libxinerama-dev libxml2-dev libxrender-dev libpulse-dev libalut-dev 

Verify
gcc --version
gcc (Ubuntu 5.4.0-6ubuntu1~16.04.10) 5.4.0 20160609

Install autobuild into python
@
-   If building lindenlab viewer64 code base use autobuild-1.1.7 */confirmed to work with linux but, versions are up to 1.1.9 at this writing/*

-       sudo pip install git+http://bitbucket.org/lindenlab/autobuild.git@v1.1.7

Install optional tools

sudo apt-get install --install-recommends git kdiff3 mc

Optionally install gcc-version 4.6 which is needed to build library archives,

sudo apt-get install --install-recommends gcc-4.6 g++-4.6 cpp-4.6

If using ssh

mkdir ~/.ssh

copy you keys to this directory

cd ~/.ssh

sudo chmod 600 id_rsa

cd ~/

sudo chmod 700 .ssh

Voice libraries are already installed on 32 bit systems so, steps below can be skipped.

Voice 32 bit libraries are not needed to build the viewer but, are needed to test voice in the viewer.

sudo dpkg --add-architecture i386

sudo apt-get update

sudo apt-get install --install-recommends libasound2:i386 libasound2-plugins:i386 libasyncns0:i386 libattr1:i386 libc6:i386 libc6-i686:i386 libcap2:i386 libdbus-1-3:i386 libflac8:i386 libgcc1:i386 libice6:i386 libidn11:i386 libjson0:i386 libogg0:i386 libpulse0:i386 libsm6:i386 libsndfile1:i386 libstdc++6:i386 libvorbis0a:i386 libvorbisenc2:i386 libwrap0:i386 libuuid1:i386 libx11-6:i386 libx11-xcb1:i386 libxau6:i386 libxcb1:i386 libxdmcp6:i386 libxext6:i386 libxi6:i386 libxtst6:i386 zlib1g:i386 
Open your vm and follow instructions for 64 bit from above. 

Below is a sample ~/.hgrc (mercurial.ini) file. This uses tortoisehg or command line mercurial, kdiff3 as a merge tool and gedit as a visual editor.
The visual editor may be changed based on personal perference.

As an option add this to you bash history file ~/.bashrc

export AUTOBUILD_PLATFORM_OVERRIDE='linux64'


-  git clone https://bitbucket.org/kokua/kokua-release

  Kokua-NORLV can be built with opensource or properity audio engine. The opensource solution uses openal for sounds. Use of the propriety FMOD Studio library for sounds and streaming audio is supported but, the FMOD Studio library must be provided separately.

-  Configure for an openal build:

Following assumes a clean build tree.

cd Kokua-release

Update the source tree to Kokua-NORLV. This is a build without RLV or if you want RLV it would be git checkout Kokua-MKRLV

git checkout Kokua-NORLV

autobuild configure -A 64 -c ReleaseOS -- -DLL_TESTS:BOOL=OFF -DUSE_KDU:BOOL=OFF -DHAVOK_TPV:BOOL=OFF -DFMODSSTUDIO:BOOL=OFF -DOPENAL:BOOL=ON -DPACKAGE:BOOL=ON 2>&1 |tee configure.log

Note: Kokua is built with various libraries which have restrictions on their distribution in source form. For this reason we do not recommend self-builds of Kokua unless it's specifically for contributing code.

- Build the viewer

-autobuild build -A 64 -c ReleaseOS 2>&1 |tee build.log

- Configuration and building typically takes 30-45 minutes for NORLV or MKRLV depending on the hardware. FTRLV and MKRLV use substantially the same code so building one after the other is much quicker with only changed files needing compilation.

- Test the build

cd build-linux-x86_64/newview/packaged

Install the viewer with

sudo ./install.sh follow the defaults

This places the viewer in /opt/kokua-install and places a Kokua menu entry under Applications->Internet

sudo is the preferred method as the chrome-sandbox a part of Chrome Embedded Framework requires root permissions.

Note: The install.sh script takes a backup of the previous installed version and may error if installs are done in quick succession. If this happens simply delete the backup folder and retry.

Windows

Below assumes a working Visual Studio 2017 installed.

Reference:https://wiki.secondlife.com/wiki/Visual_Studio_Viewer_Builds

You will need these items before you begin:

-  An installer for Windows 10 Pro 64bit

-  A valid Windows Product key

-  An installer for Visual Studio 2017

-  A valid license for Visual Studio 2017

- Install Windows 10 Pro 64bit using your own product key

- Keep running Windows Update (Start Menu -> All Programs -> Windows Update) until clicking on "Check for Updates" there tells you everything is up to date.

- Depending on the age of the install media you started with, this could take a really long time and many, many iterations.

==Microsoft Visual Studio 2017 Pro==

- Install VS 2017 Pro

- Note: If you don't own a copy of VS 2017 Pro, you might consider installing the 

- [http://www.visualstudio.com/en-us/news/vs2017-community-vs.aspx Community Version]

- Run the installer as Administrator (right click, "Run as administrator")

- Uncheck all the "Optional features to install:" - they are not required

- Download and install VS2017 Service Packs and updates

************Below needs confirmation.********************
- [http://www.visualstudio.com/en-us/downloads/download-visual-studio-vs#DownloadFamilies_5 Update 4 ] 

- is the most recent '''released''' version at time of writing (2015-01)

- Run the installer as Administrator (right click, "Run as administrator")

==DirectX SDK==

- Download and install [http://www.microsoft.com/en-us/download/details.aspx?id=6812 DirectX SDK (June 2010)]

- Run the installer as Administrator (right click, "Run as administrator")

- At the Installation Options screen, set everything except the DirectX Headers and Libs to "This feature will not be installed"

==CMake==

- Download and install [http://www.cmake.org/download/ CMake 3.18.04] (32bit is only option)

- Run the installer as Administrator (right click, "Run as administrator")

- At the "Install options" screen, select "Add CMake to the system PATH for all users"

- For everything else, use the default options (path, etc.)

==Cygwin==

- Download and install [http://cygwin.com/install.html Cygwin 64] (64bit)

- Run the installer as Administrator (right click, "Run as administrator")

- Use default options (path, components etc.) *until* you get to the "Select Packages" screen

- Add additional packages:

- Devel/patch

- Use default options for everything else

==Python==

- Download and install [https://www.python.org/ftp/python/2.7.17/python-2.7.17.msi Python 2.7.17 (32bit)] 

- Note: No option available to install as Administrator

- Use default options (path, components etc.) *until* you get to the "Customize Python" screen

- Change "Add python.exe to Path" to "Will be installed on local hard drive"

==Intermediate check==

- Confirm things are installed properly so far

- Open a Cygwin terminal and type:

-  cmake --version

-  git --version

-  python --version

- If they all report sensible values and not "Command not found" errors, then you are in good shape}}

==Set up Autobuild and Python==

-  This section only works inside the Windows Command Prompt. 

- Bootstrap pip 

-   Download (Save As) https://bootstrap.pypa.io/get-pip.py get-pip.py and copy to a temp folder

-   Open Windows Command Prompt

-   Switch to that temp folder and execute it <code>python get-pip.py</code>

-   Pip will be installed

- Bootstrap easy_install 

-   Download (Save As) https://bootstrap.pypa.io/ez_setup.py ez_setup.py and copy to a temp folder

-   Remain in Windows Command Prompt

-   Switch to that temp folder and execute it python ez_setup.py

-   easy_install will be installed

- Install Autobuild

-   Remain in Windows Command Prompt

-   Change to the Python Scripts folder that was just created

-   Typically cd \Python27\Scripts

-   If building lindenlab viewer64 code base use autobuild-1.1

-       Run pip install git+http://bitbucket.org/lindenlab/autobuild-1.1#egg=autobuild

- Update system PATH

-  Add Python Scripts folder to PATH environment variable via the Control Panel

-  Typically C:\Python27\Scripts

==NSIS (Unicode)==

-  Install V3 for packaging under Windows


==Check Paths in VS 2017==

Open Developer Command Prompt for VS2017 C:\Program Files (x86)\Microsoft Visual Studio 2017\Common7\Tools\VsDevCmd.bat

This command prompt is kinda hard to find on newer windows versions. Use File Explorer to locate VsDevCmd.bat then,

right click and send shortcut to Desktop.

In Developer Command Prompt for VS2017

Verify that these version request.

-  cmake --version

-  git --version

-  python --version

-  autobuild --version

==Get source and compile==
Use git to clone kokua-release repository to your local machine. For example: 

-   git clone https://bitbucket.org/kokua/kokua-release

-   This will place a folder with the source code typically in <pathto>/kokua-release

-   cd kokua-release 

-   git checkout Kokua-NORLV

-   autobuild configure -c ReleaseOS -A 64 -- -DCMAKE_VERBOSE_MAKEFILE:BOOL=FALSE -DLL_TESTS:BOOL=OFF -DPACKAGE:BOOL=FALSE -DOPENAL:BOOL=TRUE -DFMODSTUDIO:BOOL=OFF -DUSE_KDU:BOOL=OFF -DHAVOK_TPV:BOOL=OFF
    
    autobuild build --no-configure -c ReleaseOS -A 64
 
-  The above will produce a command line build without an install program.

-  In folder <pathto>kokua-release/build-vc150/newview/Release find and right click on file kokua-bin.exe and select Send to->Desktop (create shortcut)

-  Select Desktop and right click the kokua-bin.exe shortcut and click Properties.

-  Edit Start in to "<pathto>\kokua-release\indra\newview". This allows use of working tree skin and settings files.

-  Click on the kokua-bin.exe shortcut and the viewer should startup and run.

-  Optionally, Visual Studio 2017 program may be used to build the viewer.

-  After the autobuild configure step, start Visual Studio 2017 and open the solution from <pathto>\Kokua-release\build-vc150\Kokua.sln

-  Follow the steps below to confirm or set the Start up project and Debugging working directory...

-  In the Solution Explorer click on kokua-bin, then from the main menu Project select Set as startup project.    

-  From main menu Project open kokua-bin Properties, then in kokua-bin Properties window chose Debugging, then Working directory.

-  At the right side a down arrow expanders. From there use Browse or Edit and set to <pathto>/kokua-release/build-vc150/newview/Release

-  And Apply to write the property page to memory.

-  Under main menu DEBUG is a right pointing green arrow with text 'Local Windows Debugger' click to start building...

-  Once the build competes the viewer should be at the start page waiting to logon.
--------

Mac

--------

The current development environment is XCode 12.2 running on OS X 11.0.1 where the viewer is built 64-bit with OS X SDK 11.0 and Deployment Target of OS X 10.9.


You can both build from the command line in terminal or build the XCode project that is generated during configuration.

NOTE: On Upgrading XCode to a new version you probably should delete the Derived Data folder that is found in Development/XCode as it may contain links to old system library locations preventing your build from linking.
	
    -- 
There are two programs external to MacOS that allow other programming tools installation that do not touch MacOS installer methods.

These are macports https://www.macports.org and homebrew https://brew.sh . 

Linden’s version of autobuild requires a different version of Python than the MacOS system installed so the best way to get it installed is to first install MacPorts or HomeBrew.
 
With MacPorts installed, in terminal install the following ports:

• sudo port install python27

• sudo port install py27-pip

• sudo port install cmake

When prompted by the installer run python_select to use the version you just installed. It will be installed in /opt/local/bin

HomeBrew has the advantage by using similar commands for Linux and MacOS.

•  brew install python27

•  brew install py27-pip

•  brew install cmake


Homebrew installs packages to their own directory and the symlinks their files into /usr/local.

If you have newer versions of Xcode installed then you also need to run xcode-select to make sure you use currently installed Xcode for your build.

--

Xcode should have created the directory ~/Library/Developer during installation. If not create it (or use the location of your choice) and shortcut it to the Finder sidebar. 

In terminal cd to the above directory and type the command:

Using Pip Install auto build python dependencies by typing:

sudo pip install ‘git+https://bitbucket.org/lindenlab/autobuild-1.1#egg=autobuild'

If everything goes well it should be installed in /opt/local/Library/Frameworks/Python.framework/Versions/2.7/bin/autobuild

To make life easier edit your .bash_profile and add the lines

alias autobuild="/opt/local/Library/Frameworks/Python.framework/Versions/2.7/bin/autobuild"

export AUTOBUILD=/opt/local/Library/Frameworks/Python.framework/Versions/2.7/bin/autobuild

Then source your.bash_profile

--

To verify your build environment the best way forward is most likely to download the LL source by following the instructions on http://wiki.secondlife.com/wiki/Compiling_the_viewer_(Mac_OS_X_XCode_6.1)

You should be able to both use the Xcode project (easiest to verify) and the command line build. 

NOTE: Regardless of which configuration option you use on the command line the Xcode project will have the build mode set to Debug. To change this go to Product > Scheme > Edit Scheme (with ALL_BUILD selected) and change the Build Configuration to RelWithDebInfo or Release respectively

BUILD NOTE: When building in Xcode at some point the build will fail because it cannot find packages-info.txt. At this point just restart the build and it will continue from there.

The root cause of this is that it tries to run autobuild by spawning a shell from inside autobuild, but Xcode will not allow any other version than the system python to be called so autobuild will fail - it does not even find it.  For anything but a (final) release build this is not significant. This build has to be done from the command line. 

--

To build Kokua first download the Kokua source code with the following command in terminal:

For the SecondLife version:

git clone https://bitbucket.org/kokua/kokua-release 

You can configure the build with:

autobuild configure -c ReleaseOS -A 64 -- -DCMAKE_VERBOSE_MAKEFILE:BOOL=FALSE -DLL_TESTS:BOOL=OFF -DPACKAGE:BOOL=FALSE -DOPENAL:BOOL=TRUE -DFMODSTUDIO:BOOL=OFF -DUSE_KDU:BOOL=OFF -DHAVOK_TPV:BOOL=OFF

or 

autobuild configure -c RelWithDebInfoOS -A 64 -- -DCMAKE_VERBOSE_MAKEFILE:BOOL=FALSE -DLL_TESTS:BOOL=OFF -DPACKAGE:BOOL=FALSE -DOPENAL:BOOL=TRUE -DFMODSTUDIO:BOOL=OFF -DUSE_KDU:BOOL=OFF -DHAVOK_TPV:BOOL=OFF

When you have made sure your configuration is working (and compiles in Xcode) you can also compile on the command line by substituting configure with build in the two commands above. 

Disclaimer

-----

-      This software is not provided nor supported by Linden Lab, the makers of Second Life.







