**********************************************************************
 	
	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:	e-mail:		baggior@libero.it		
			web-page:	digilander.iol.it/baggior

 	Copyright (c) Marco Tombesi, 2001

	This demo is not Linux-compatible.

**********************************************************************

---------------------
 "Sources\" Contents:
---------------------
.\Debug\		once compiled "Skeldemo.exe" goes here
.\Plugins\		once compiled "MRCexport.dle" goes here
.\MRCExport\		contains sources for MAX export plugin
.\ImportMRC\		contains sources for MRC file loader DLL

.\Data\Test.mrc		the file you can test while developing

.\skeldemo.dsw		Global demo Workspace file 
			(it includes every single project: Skeldemo, MRCExport, ImportMRC)

.\*.cpp or *.h		sources for skeldemo
.\MRCimport.dll		once compiled it is the importMRC DLL
.\MRCimport.lib		needed for proper dynamic linking
.\utils.lib		needed implementation of some useful functions
.\msvcrt.dll		standard C libraries (may be needed)




---------------
  Compilation:
---------------
You need visual C++ to compile skeldemo and importMRC, 
and you need 3d studio Max 3 SDK to compile MRCExport plugin.

NOTE:
In MRCExport settings is assumed that every SDK library file is found in:
"C:\3DSMAX3\Maxsdk\lib"
if is not, change the appropriate setting.



