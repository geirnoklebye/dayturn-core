Kokua - Linux Voice Support README
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

WHAT IS IT?
-=-=-=-=-=-

Linux Voice Support is a feature in testing which allows users of the Linux
on Kokua and other Second Life clients to participate in voice-chat with 
other residents and groups inside Metaverse Grids, with an appropriate
headset/microphone.

REQUIREMENTS
-=-=-=-=-=-=

* A headset/microphone supported by your chosen version of Linux
* At this time, the PulseAudio audio system is recommended; this software
  is already part of most modern (2009+) Linux desktop systems.  Alternatively,
  the ALSA audio system may be used on systems installed from around
  2007 onwards (again this is likely already installed on your system).
* Linux 32 bit voice should work as delivered. To determine if all libraries
  are present: Open a terminal and:
  cd /home/<user>/.kokua-install (or the directory where Kokua is installed)
  LD_LIBRARY_PATH=${PWD}/lib ldd  bin/SLVoice
  Review the listing and check for any not found shared (*.so) libraries.
  Use your package manager to resolve the not found items.
* Linux 64 bit on a Multi-Arch system requires additional steps to obtain voice
  because voice as delivered by Vivox is 32 bit based.
  First, to check your system not found shared (*.so) libraries:
  cd /home/<user>/.kokua-install (or the directory where Kokua is installed)
  LD_LIBRARY_PATH=${PWD}/lib32 ldd  bin/SLVoice
  If you have never added the 32 bit Arch there will be many not found.
  To add the 32 bit Arch proceed as follows:
  sudo dpkg --add-architecture i386
  sudo apt-get update
  sudo apt-get install libasound2:i386 libasound2-plugins:i386 libasyncns0:i386 \
libattr1:i386 libc6:i386 libc6-i686:i386 libcap2:i386 libdbus-1-3:i386 libflac8:i386 \
libgcc1:i386 libice6:i386 libidn11:i386 libjson0:i386 libogg0:i386 libpulse0:i386 \
libsm6:i386 libsndfile1:i386 libstdc++6:i386 libvorbis0a:i386 libvorbisenc2:i386 \
libwrap0:i386 libx11-6:i386 libx11-xcb1:i386 libxau6:i386 libxcb1:i386 \
libxdmcp6:i386 libxext6:i386 libxi6:i386 libxtst6:i386 zlib1g:i386
  Repeat LD_LIBRARY_PATH=${PWD}/lib32 ldd  bin/SLVoice 
  and use the system package manager to add missing libraries.

TESTING YOUR SETTINGS
-=-=-=-=-=-=-=-=-=-=-

* The Second Life region 'Voice Echo Canyon' is a great place for testing
your hardware settings and quality - it will 'echo' your voice back to you
when you speak.

KNOWN PROBLEMS
-=-=-=-=-=-=-=

* Compatibility with old ALSA-based audio systems (such as Ubuntu Dapper
  from 2006) is poor.

TROUBLESHOOTING
-=-=-=-=-=-=-=-

PROBLEM 1: I don't see a white dot over the head of my avatar or other
  Voice-using avatars.
SOLUTION:
a. Ensure that 'Enable voice' is enabled in the 'Sound' section of the
  Preferences window, and that you are in a voice-enabled area.
b. If the above does not help, exit Kokua and ensure that any
  remaining 'SLVoice' processes (as reported by 'ps', 'top' or similar)
  are killed before restarting.

PROBLEM 2: I have a white dot over my head but I never see (or hear!) anyone
  except myself listed in the Active Speakers dialog when I'm sure that other
  residents nearby are active Voice users.
SOLUTION: This is an incompatibility between the Voice support and your
  system's audio (ALSA) driver version/configuration.
a. Back-up and remove your ~/.asoundrc file, re-test.
b. Check for updates to your kernel, kernel modules and ALSA-related
  packages using your Linux distribution's package-manager - install these,
  reboot and re-test.
c. Update to the latest version of ALSA manually.  For a guide, see the
  'Update to the Latest Version of ALSA' section of this page:
  <https://help.ubuntu.com/community/HdaIntelSoundHowto> or the official
  documentation on the ALSA site: <http://www.alsa-project.org/> - reboot
  and re-test.

PROBLEM 3: I can hear other people, but they cannot hear me.
SOLUTION:
a. Ensure that you have the 'Speak' button (at the bottom of the Kokua
   window) activated while you are trying to speak.
b. Ensure that your microphone jack is inserted into the correct socket of your
  sound card, where appropriate.
c. Use your system mixer-setting program (such as the PulseAudio 'volume
  control' applet or the ALSA 'alsamixer' program) to ensure that microphone
  input is set as the active input source and is not muted.
d. Verify that audio input works in other applications, i.e. Audacity

PROBLEM 4: Other people just hear bursts of loud noise when I speak.
SOLUTION:
a. Use your system mixer-setting program or the 'alsamixer' program to ensure
  that microphone Gain/Boost is not set too high.

FURTHER PROBLEMS?
-=-=-=-=-=-=-=-=-

Please report further issues to the public Kokua issue-tracker
at <https://sourceforge.net/p/team-purple/kokua/tickets/> 
(please note, however, that for Kokua this is for support issues).
