#  /Fefilename  	sets the name of the executable
#  /Idirectory    	sets a single include directory
#  /Ddefine             defines a macro

OPTIONS=/nologo /Fefiledmp.exe /I..\..\hecvm /DWINDOWS_32

FILEDMP.EXE:
	CL.EXE $(OPTIONS) filedmp.c $(LINK_OPTIONS)