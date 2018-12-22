**********************************************************************
 	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:	e-mail:		baggior@libero.it		
			web-page:	digilander.iol.it/baggior

 	Copyright (c) Marco Tombesi, 2001

	This demo is not Linux-compatible.

**********************************************************************
---------------
 Demo Contents:
---------------
.\Sources\		workspace and sources for every demo's module 
			(look to inner ReadMe.txt for compilation instructions)
.\Plugin\		precompiled plugin "MRCexport.dle"

.\Skeldemo.exe		precompiled demo application (RUN IT)
.\MRCimport.dll		precompiled import DLL
.\msvcrt.dll		standard C libraries (needed)
.\Data\Test.mrc		already exported animation data(overwrite this with yours to view it)
.\Data\Test.bak		already exported animation data(bakup file)
.\Data\Test.max		original max 3 file

**********************************************************************

NOTE: This demo requires the use of the Max.h file included with
the 3DS Max SDK. 

**********************************************************************
---------------		MRC Export plugin	--------------
			for 3DS MAX 3.1
**********************************************************************
---------------
 Installation:
---------------
Copy file "MRCexport.dle" from ".\Plugin" into folder:

	"\<MAX_HOME>\plugins\"

---------------
 plugin Use:
---------------
Select the single mesh your character is composed of and choose file\export on menu bar

Then chose this output format
	"MRCExport v.1.22"

name the file "Test.MRC"

In the dialog that appears chose sample rate in millisecs.

---------------------
 plugin Limitations:
---------------------

IMPORTANT NOTES:
----------------
This plugin is compatible with MAX ver 3 and 3.1 but NOT with MAX 4.
Check for Newer version at my home site.

The max animation must submit to the following restrictions
-> USE BIPED or Bones to animate and use Physique to assign influence
-> Not USE FOOTSTEPS controller
-> Be a single selectable mesh object
-> Animation must start from 0
-> the biped (or bonetree) must be perfectly centered internally into the mesh
-> all the mesh must be assigned to a single smoothing group




**********************************************************************
---------------		SkelDemo	--------------------
	  	MRC format visualizator
**********************************************************************

There is a test file to try, in folder "\<SkelDemo_HOME>\Data\", called "Test.MRC"
(a backup copy of this is also there: "Test.bak").
In the same dir there is also the original Max file wich it is exported from.


If you want to play another file (one that you could grab from a max project via MRCexport plugin)
just overwrite the above Test.MRC with the new one. (NOTE: use that name)


Run SkelDemo.exe to play the exported model.

Keys: 
	[F1]	swap fullscreen/windows
	[ESC]	Quit demo


IMPORTANT NOTES:
---------------
-> there is no smart camera positioning routine into this demo, 
   so in the precompiled "Skeldemo.exe" the camere points to (0,0,0) 
   and has a limitated view field. If your exported animation is outside
   this bounds it will not be framed.
   Change source's lines in file OpGl.cpp to properly handle your model 
   otherwise rescale and reposition it accordingly.
   (with default settings are framed objects centered on 0,0,0 and having an extension of about 300,800,600, 
   along x, y, z respectively.)
-> SkelDemo.exe is compiled without any kind of optimization. If you'll recompile 
   this application using inline expansion and fast code optimization,
   you'll find great FPS improvements.
   Many other code enhancements may be done, first of all you can use vertex arrays
   instead of simple triangle draw...


...enjoy
