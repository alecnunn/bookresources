#  /Fefilename  	sets the name of the executable
#  /Idirectory    	sets a single include directory

OPTIONS=/nologo /Fehashtbl.exe /I..\..\hecvm 

HASHTBL.EXE:
	CL.EXE $(OPTIONS) HASHTBL.CPP $(LINK_OPTIONS)