# SETUP
=======
This document should help you set-up the "Serpents Watcher" project.

## Prerequisites
----------------
* CMake (tested with 3.1)
* wxWidgets (tested with version 3.0.2)
* guslib (tested with guslib 1.8.0)
* Visual Studio (tested with Visual Studio 2013 Express)

### wxWidgets
-------------
If you do not have wxWidgets already built on your machine, you'll have to build it yourself... follow these steps:
*	Go to http://wxwidgets.org/downloads/. Download and install the WxWidgets 3.0.2. (present it's Version 3.0.2 and windows installer edition is recommended). 
		As recommended in the documentation, install it without space in the path name, so I simply use the default: "C:\wxWidgets-3.0.2".
*	Open the Advanced System Settings and click on the "Environment Variables...".
		Add a New item: Variable name: WXWIN, Variable value: C:\wxWidgets-3.0.2 (path of your wxwidgets)
*	Go to your wxWidgets folder (here, for example, C:\wxWidgets-3.0.2), open the "\build\msw\wx_vc10.sln" file (or more recent version if you have it ex: wx_vc12.sln)
		with visual studio (just double click on it).  If prompted with some update of the files, just hit "OK". 
*	Build this solution (hit F7 or from menu) for "Debug" and "Release" separately(choose the build type, 
		each build process needs may be 1~2 min). (Note: If you need DLL, just build the DLL Debug and DLL Release). 
		It should build without error and show something like"========== Build: 23 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========".
*	Now go to your wxWidgets folder and open "\samples\minimal\minimal.dsw" with visual studio (double click only). 
		Again,  if prompted with some update of the files, 
		just hit "OK". Then build it (hit F7, or F5 for debug mode) for debug or release to test it works OK or not. 
		It should build without error pretty quickly and now you have a new folder called "vc_mswud" under "\samples\minimal\vc_mswud", 
		where you would find a setup called "minimal.exe".
*	Double click it to see this setup. When you close the Visual Studio, it will ask you to save the change or not, just click Yes and save it to the same folder.
		Done.

### CMake
---------
Download and install CMake (http://www.cmake.org/download/).
You will need CMake to build **guslib**.
Other versions might work. You'll have to test which ones yourself.

### Visual Studio
-----------------
Download and install Visual Studio

### guslib
----------
Download the Guslib library from git hub ( https://github.com/dezGusty/guslib ) and build it
	a.	Create a new environment variable with the name **GUSLIB_DIR** and the value: <path to the guslib directory>\guslib
		(example: the **GUSLIB_DIR** environment variable on my system is set to C:\Users\Petru.barko\Documents\Projects\guslib )




## Building
-----------
Open Serpents.sln and build the project. It should work.
	a. If it did not work click on Project and select Manage NuGet Packages. Then search for MS Visual Studio wxWidgets Helper and install it. That should do it.