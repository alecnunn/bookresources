#  /Fefilename  sets the name of the executable
#  /Idirectory  sets a single include directory
#  /Ddefine	defines a macro

OPTIONS=/nologo /Fehasm.exe /I.\ /DWINDOWS_32 

HASM.EXE:
	CL.EXE $(OPTIONS) main.cpp