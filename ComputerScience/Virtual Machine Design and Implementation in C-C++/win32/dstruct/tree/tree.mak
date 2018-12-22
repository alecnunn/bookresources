#  /Fefilename  	sets the name of the executable
#  /Idirectory    	sets a single include directory

OPTIONS=/nologo /Fetree.exe /I..\..\hecvm 

TREE.EXE:
	CL.EXE $(OPTIONS) tree.cpp $(LINK_OPTIONS)