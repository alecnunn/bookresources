#  /Fefilename  	sets the name of the executable
#  /Idirectory    	sets a single include directory
#  /Ddefine		defines a macro

OPTIONS=/nologo /Fenumfmt.exe /I..\..\hecvm /DWINDOWS_32

NUMFMT.EXE:
	CL.EXE $(OPTIONS) numfmt.c $(LINK_OPTIONS)