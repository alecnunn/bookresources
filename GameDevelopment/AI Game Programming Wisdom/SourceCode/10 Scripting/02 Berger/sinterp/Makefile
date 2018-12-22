# All compiler and linker generated files will be written to this directory.
OUTDIR=Debug

# Specify all of the object files that are needed to build this executable.
OBJS= \
	$(OUTDIR)\VM.obj \
	$(OUTDIR)\sinterp.obj

# Define the name of the compiler and its command-line arguments.
CPP=cl
CPP_FLAGS=/nologo /MLd /WX /GX /Zi /Od /I. /Fo$(OUTDIR)\ /Fd$(OUTDIR)\sinterp.pdb /FD /GZ /c

# Define the name of the linker and its command-line arguments.
LINK=link
LINK_FLAGS=/nologo /incremental:no /pdb:$(OUTDIR)\sinterp.pdb /debug /out:$(OUTDIR)\sinterp.exe


# This is the main target of the Makefile.  It builds everything.
all : $(OUTDIR)\sinterp.exe


# This target deletes all of the generated files.
clean :
	-@erase $(OBJS)
	-@erase $(OUTDIR)\sinterp.exe
	-@erase $(OUTDIR)\sinterp.ilk
	-@erase $(OUTDIR)\sinterp.pdb
	-@erase $(OUTDIR)\sinterp.idb


# This next target creates the output directory, if needed.
!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

$(OUTDIR) :
    if not exist $(OUTDIR)/$(NULL) mkdir $(OUTDIR)


# This target actually builds the executable.
$(OUTDIR)\sinterp.exe : $(OUTDIR) $(OBJS)
    $(LINK) $(LINK_FLAGS) $(OBJS)


################################################################
# These rules specify all of the file dependencies.

.cpp{$(OUTDIR)}.obj::
   $(CPP) $(CPP_FLAGS) $<

VM.cpp : VM.H

VM.H : Opcode.H

sinterp.cpp : VM.H
