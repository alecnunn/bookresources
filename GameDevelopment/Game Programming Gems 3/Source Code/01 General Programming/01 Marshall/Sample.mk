##############################################################################
#
# To build:
#
#      NMAKE /F Sample.mk
#
##############################################################################

# set debugging option; full debugging is default
#NODEBUG=true

APPVER   = 4.0
CPU      = i386
TARGETOS = WINNT

!include <NTwin32.mak>


##############################################################################
#
# Project Name:   Sample
# Makefile:       Sample.mk
# Linkfile:       Sample.lnk
#
# Flags:
#   dbg
#
# Object directory:   $(CPU)
#
# Source files:
#   [ 0] Sample.cpp
#   [ 1] Scheduler.cpp
#   [ 2] Clock.cpp
#
# Library files:
#   [ 0] winmm.lib
#
##############################################################################
!message
!message CC sample scheduler clock -g -lwinmm
!message
##############################################################################

DIR_OBJ = $(CPU)
PROJECT  = Sample
MAKEFILE = Sample.mk
LINKFILE = $(DIR_OBJ)\Sample.lnk
OUT_FILE = Sample.exe
MAP_FILE = Sample.map
LOG_FILE = Sample.log
HDRS     = 
USER_INC = 

MAKE     = nmake /NOLOGO
#cc      = cl /nologo
#rc      = rc
#link    = link /NOLOGO
#implib  = lib32
#hc      = hcrtf -x
mtl      = mktyplib

# Debugging Options
#
# /Od  no optimization (simplifies debugging)
# /Yd  
# /Z7  include full symbolic debug info (was /Zi)
# /Zd  include line numbers only
# /Zi  produces PDB
#
# /W3  sets warning level to 3
#
# Linker:
#
# /DEBUG           includes debugging info (generates PDB by default)
# /DEBUGTYPE:COFF  cannot be used with PDB
# /DEBUGTYPE:CV    (default)
# /DEBUGTYPE:BOTH  cannot be used with PDB
# /PDB:NONE        forces debug info into EXE and LIB files
# /MAP             generate mapfile
#
# Debugging info in OBJ modules (line numbers only):
#
#   Compile:  /Zd /Yd
#   Link:     /DEBUG /PDB:NONE
#
# Debugging info in OBJ modules (symbolic):
#
#   Compile:  /Z7 /Yd
#   Link:     /DEBUG /PDB:NONE
#
# Debugging info in PDB file (symbolic)
#
#   Compile:  /Zi
#   Link:     /DEBUG /DEBUGTYPE:CV
#
#
ENTRY = -entry:mainCRTStartup
CC_OPT   = /GX    $(cflags) $(cdebug) $(cvarsmt)  $(USER_INC)
RC_OPT   = $(rcflags) $(rcvars) $(USER_INC)
LK_OPT   = $(ldebug) $(conlflags) 
LB_OPT   = /DEBUGTYPE:cv 

OBJ =\
      $(DIR_OBJ)\Sample.obj\
      $(DIR_OBJ)\Scheduler.obj\
      $(DIR_OBJ)\Clock.obj

STD_LIBS = $(conlibsmt) winmm.lib
USR_LIBS = 

##############################################################################
#
# rules
#
##############################################################################

build: mkdirs $(OUT_FILE)

mkdirs:
	@if not exist $(DIR_OBJ) mkdir $(DIR_OBJ)

$(OUT_FILE): $(OBJ) $(USR_LIBS) $(HDRS)
	$(link) $(LK_OPT) -out:$(OUT_FILE) @<<$(LINKFILE)
$(OBJ)
$(USR_LIBS)
$(STD_LIBS)
<<KEEP

$(DIR_OBJ)\Sample.obj: Sample.cpp Scheduler.h ITask.h Clock.h
        $(cc) $(CC_OPT) /c /Tp Sample.cpp /Fo$(DIR_OBJ)\Sample.obj

$(DIR_OBJ)\Scheduler.obj: Scheduler.cpp Scheduler.h ITask.h Clock.h
        $(cc) $(CC_OPT) /c /Tp Scheduler.cpp /Fo$(DIR_OBJ)\Scheduler.obj

$(DIR_OBJ)\Clock.obj: Clock.cpp Clock.h
        $(cc) $(CC_OPT) /c /Tp Clock.cpp /Fo$(DIR_OBJ)\Clock.obj

##############################################################################
#
# Delete the specific OBJECT files associated with this project
#  (does not delete library object files)
#
clean:
	@if exist $(DIR_OBJ)\Sample.obj del $(DIR_OBJ)\Sample.obj
	@if exist $(DIR_OBJ)\Scheduler.obj del $(DIR_OBJ)\Scheduler.obj
	@if exist $(DIR_OBJ)\Clock.obj del $(DIR_OBJ)\Clock.obj
	@if exist *.pch del *.pch

#
# deletes all object files and misc stuff -- OBJ,RES,SBR,VCP,EXP
# including precompiled headers (PCH) and debug database (PDB)
#
#
new: clean
	@if exist *.obj del *.obj
	@if exist *.res del *.res
	@if exist *.sbr del *.sbr
	@if exist *.pch del *.pch
	@if exist *.pdb del *.pdb
	@if exist *.vcp del *.vcp
	@if exist *.exp del *.exp

#
# deletes everything I can think of -- EXE, MAP, LOG, DLL, LIB
# leaves only source files intact
#
wipe: new
	@if exist *.lib del *.lib
	@if exist *.dll del *.dll
	@if exist *.exe del *.exe
	@if exist *.map del *.map
	@if exist *.log del *.log

.C.OBJ:
	$(cc) $(COPT) /c /Tc $<

.CPP.OBJ:
	$(cc) $(COPT) /c /Tp $<

