**************************************************************
*                                                            *
* Installation and setup guide for the Kokua Viewer on Linux *
*                                                            *
**************************************************************

Written by: Topaz Harley
topaz.harley@gmail.com




INTRODUCTION:
*************

Firstly let me state that I am not a member of the Kokua Development Team but an avid fan of the viewer and regular beta tester. I have written this brief guide to help others set up their linux systems to run the Kokua Viewer successfully.

Therefore any errors, omissions or other faux pas are mine and mine alone :-)


I have included commands for 3 of the most popular linux distros;

	Arch Linux (my personal favourite and default system)

	Fedora 22

	Debian 9 'Stretch'

If you are using another distribution the installation is similar, just use your package search to find the required libraries and packages.




REQUIRED PACKAGES:
******************

Depending upon the linux distribution and desktop environment you use, many of these packages may already be installed.

	
	OpenAL			a cross-platform 3D audio library
	FreeALUT		OpenAL Utility Toolkit
	GLU			Mesa OpenGL Utility Library
	FreeGLUT		OpenGL Utility Toolkit
	pangox-compat		X Window System font support for Pango
	Gstreamer0.10		GStreamer Multimedia Framework (plus plugins)
	Flash plugin		Adobe's Flash player
	QT webkit		An open source web browser engine (Qt port)
	APR			Apache Portable Runtime
	APR-util		Apache Portable Runtime utilities
	JRE			Java Runtime Environment



PACKAGE INSTALLATION:
*********************


ARCH LINUX:
***********

	sudo pacman -S openal glu freealut freeglut pangox-compat gstreamer0.10 gstreamer0.10-bad-plugins gstreamer0.10-base-plugins gstreamer0.10-good-plugins gstreamer0.10-ugly-plugins flashplugin qtwebkit apr apr-util jre8-openjdk




FEDORA 22:
**********
(for previous Fedora versions use yum in place of dnf)


	sudo dnf install openal-soft mesa-libGLU freealut freeglut pangox-compat gstreamer gstreamer-plugins-bad gstreamer-plugins-base gstreamer-plugins-good gstreamer-plugins-ugly flash-plugin apr apr-util jre1.8.0_51

	(note:JRE version will change, this was the current version at the time of writing)




DEBIAN 9 'Stretch'
******************

	sudo apt-get install libopenal1 libglu1-mesa libalut0 freeglut3 libpangox gstreamer0.10 gstreamer0.10-plugins-bad gstreamer0.10-plugins-base gstreamer0.10-plugins-good gstreamer0.10-plugins-ugly flashplugin-nonfree libqtwebkit4 libapr1 libaprutil1 openjdk-8-jre



KOKUA INSTALLATION:
*******************

Open a terminal in the extracted Kokua directory and install with the following command;


	sudo ./install.sh

I have always found it best to install with sudo and accept the default installation directory ie: /opt/kokua-install



Now run the following command to check which libraries are missing, there will be at least one, this is normal.


	ldd /opt/kokua-install/bin/do-not-directly-run-kokua-bin


You will be presented with a list of 'found' and 'not found' libraries. Typically libGLOD, libopenjpeg and libpng12, although not in all instances. These libraries are included with the viewer and can be found in /opt/kokua-install/lib64.



Next we will create a symlink to the missing libraries. In this example libGLOD


ARCH LINUX
**********

	sudo ln -s /opt/kokua-install/lib64/libGLOD.so /lib64/libGLOD.so

FEDORA 22
*********

	sudo ln -s /opt/kokua-install/lib64/libGLOD.so /lib64/libGLOD.so


DEBIAN 9 'Stretch'
******************

	sudo ln -s /opt/kokua-install/lib64/libGLOD.so /lib64/libGLOD.so


Now run the ldd command again and check that libGLOD has now been found:


	ldd /opt/kokua-install/bin/do-not-directly-run-kokua-bin


Repeat the above steps for any other library that is missing and is included in the Kokua /lib64 directory. Typically these are libpng12 and libopenjpeg, both are included in the Kokua library folder or are readily available in your distribution's repositories.


FINALLY:
********

You should now be able to run Kokua from it's menu entry or from a terminal with:

	sh /opt/kokua-install/kokua


I hope you found this guide useful and that Kokua is now running perfectly on Linux.

Enjoy SecondLife! (or whichever grid you frequent) See you inworld !


Topaz Harley 
(aka Toppy)





	


			
	
	
	



