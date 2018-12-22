#  /Fefilename  	sets the name of the executable
#  /Idirectory    	sets a single include directory
#  /link		sets linker options to include extra libraries
#  /Ddefine		defines a macro

OPTIONS=/nologo /FeHECVM.EXE /I.\ /DWINDOWS_32  
LINK_OPTIONS=/link /DEFAULTLIB:ws2_32.lib

HECVM.EXE:
	CL.EXE $(OPTIONS) main.c $(LINK_OPTIONS)