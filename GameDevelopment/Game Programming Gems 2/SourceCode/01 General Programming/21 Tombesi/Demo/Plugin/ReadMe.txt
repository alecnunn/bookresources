**********************************************************************
 	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:	e-mail:		baggior@libero.it		
			web-page:	digilander.iol.it/baggior

 	Copyright (c) Marco Tombesi, 2001

	This demo is not Linux-compatible.

**********************************************************************



**********************************************************************
---------------		MRC Export plugin	--------------
			for 3DS MAX 3.1
**********************************************************************

---------------
 Installation:
---------------
Copy the file "MRCexport.dle" 

into folder:

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
