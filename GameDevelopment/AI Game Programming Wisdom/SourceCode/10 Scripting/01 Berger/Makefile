# All compiler and linker generated files will be written to this directory.
OUTDIR=Debug

# Specify all of the object files that are needed to build this executable.
OBJS= \
	$(OUTDIR)\scc.obj \
	$(OUTDIR)\scc-lexer.obj \
	$(OUTDIR)\scc-parser.obj \
	$(OUTDIR)\CodeGen.obj \
	$(OUTDIR)\PTNode.obj \
	$(OUTDIR)\SmartPtr.obj

# Define the name of the compiler and its command-line arguments.
CPP=cl
CPP_FLAGS=/nologo /MLd /WX /GX /Zi /Od /I. /Fo$(OUTDIR)\ /Fd$(OUTDIR)\scc.pdb /FD /GZ /c

# Define the name of the linker and its command-line arguments.
LINK=link
LINK_FLAGS=/nologo /incremental:no /pdb:$(OUTDIR)\scc.pdb /debug /out:$(OUTDIR)\scc.exe


# This is the main target of the Makefile.  It builds everything.
all : scc-parser.h scc-parser.cpp scc-lexer.cpp $(OUTDIR)\scc.exe


# This target deletes all of the generated files.
clean :
	-@erase $(OBJS)
	-@erase $(OUTDIR)\scc.exe
	-@erase $(OUTDIR)\scc.ilk
	-@erase $(OUTDIR)\scc.pdb
	-@erase $(OUTDIR)\scc.idb
	-@erase scc-lexer.cpp
	-@erase scc-parser.cpp
	-@erase scc-parser.h


# This next target creates the output directory, if needed.
!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

$(OUTDIR) :
    if not exist $(OUTDIR)/$(NULL) mkdir $(OUTDIR)


# This target actually builds the executable.
$(OUTDIR)\scc.exe : $(OUTDIR) $(OBJS)
    $(LINK) $(LINK_FLAGS) $(OBJS)


################################################################
# These rules define how to build the C++ source files from the Flex and Bison
# source files.  Note that we have to do a little file renaming magic with
# Bison to get the auto-generated header file the way we like it.

scc-lexer.cpp : scc-lexer.l $(OUTDIR)
	flex -oscc-lexer.cpp scc-lexer.l

scc-parser.cpp scc-parser.h : scc-parser.y $(OUTDIR)
	-@erase scc-parser.h
	bison -d -oscc-parser.cpp scc-parser.y
	-@rename scc-parser.cpp.h scc-parser.h


################################################################
# These rules specify all of the file dependencies.

.cpp{$(OUTDIR)}.obj::
   $(CPP) $(CPP_FLAGS) $<

CodeGen.H : Opcode.H PTNode.H

CodeGen.cpp : CodeGen.H PTNode.H

PTNode.H : SmartPtr.H

PTNode.cpp : PTNode.H

SCC.cpp : SCC.H

SmartPtr.cpp : SmartPtr.H

scc-lexer.l : SCC.H PTNode.H scc-parser.h

scc-parser.y : SCC.H PTNode.H CodeGen.H
