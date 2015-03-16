SETUP:

1. If you do not have wxWidgets you have to install it... follow these steps:
	a.	Go to http://wxwidgets.org/downloads/. Download and install the WxWidgets 3.0.2. (present it's Version 3.0.2 and windows installer edition is recommended). 
		As recommended in the documentation, install it without space in the path name, so I simply use the default: "C:\wxWidgets-3.0.2".
	b.	Open the Advanced System Settings and click on the "Environment Variables...".
		Add a New item: Variable name: WXWIN, Variable value: C:\wxWidgets-3.0.2 (path of your wxwidgets)
	c.	Go to your wxWidgets folder (here, for example, C:\wxWidgets-3.0.2), open the "\build\msw\wx_vc10.sln" file (or more recent version if you have it ex: wx_vc12.sln)
		with visual studio (just double click on it).  If prompted with some update of the files, just hit "OK". 
	d.	Build this solution (hit F7 or from menu) for "Debug" and "Release" separately(choose the build type, 
		each build process needs may be 1~2 min). (Note: If you need DLL, just build the DLL Debug and DLL Release). 
		It should build without error and show something like"========== Build: 23 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========".
	e.	Now go to your wxWidgets folder and open "\samples\minimal\minimal.dsw" with visual studio (double click only). 
		Again,  if prompted with some update of the files, 
		just hit "OK". Then build it (hit F7, or F5 for debug mode) for debug or release to test it works OK or not. 
		It should build without error pretty quickly and now you have a new folder called "vc_mswud" under "\samples\minimal\vc_mswud", 
		where you would find a setup called "minimal.exe".
	f.	Double click it to see this setup. When you close the Visual Studio, it will ask you to save the change or not, just click Yes and save it to the same folder.
		Done.
2. Download the Guslib library from git hub ( https://github.com/dezGusty/guslib ) and build it
	a.	Create a new environment variable with the name GUS_LIB and the value: <path to the guslib directory>\guslib-master\guslib-master 
		(example: the GUS_LIB environment variable on my system is set to D:\petru\guslib-master\guslib-master )
3.	Open Serpents.sln and build the project. It should work.
	a. If it did not work click on Project and select Manage NuGet Packages. Then search for MS Visual Studio wxWidgets Helper and install it. That should do it.