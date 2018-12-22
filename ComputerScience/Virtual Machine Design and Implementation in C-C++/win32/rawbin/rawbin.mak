#  /Fefilename  	sets the name of the executable
#  /Idirectory    	sets a single include directory
#  /Ddefine             defines a macro

OPTIONS=/nologo /Ferawbin.exe /I..\hecvm /DWINDOWS_32  

RAWBIN.EXE:
	CL.EXE $(OPTIONS) rawbin.c $(LINK_OPTIONS)