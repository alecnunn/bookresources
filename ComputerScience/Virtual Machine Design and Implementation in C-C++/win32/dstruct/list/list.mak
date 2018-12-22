#  /Fefilename  	sets the name of the executable
#  /Idirectory    	sets a single include directory

OPTIONS=/nologo /Felist.exe /I..\..\hecvm 

LIST.EXE:
	CL.EXE $(OPTIONS) LIST.CPP $(LINK_OPTIONS)