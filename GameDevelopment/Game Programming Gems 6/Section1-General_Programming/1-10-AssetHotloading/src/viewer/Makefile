COMPILER = g++
INC = 
CCFLAGS = -g -Wall $(INC)
LINKER = g++
LINKFLAGS += -L/usr/X11R6/lib
LINKLIBS +=  -lglut -lGL -lGLU

DEPEND = makedepend
.SUFFIXES: .o .cpp

program = ../bin/viewer
src = viewer.cpp TextureResource.cpp


objects = $(patsubst %.cpp, %.o, $(src))

all: depend $(program)

$(program): $(objects)
	$(LINKER) -o $@ $^ $(LINKFLAGS) $(LINKLIBS)

.cpp.o:
	$(COMPILER) $(CCFLAGS) -c $<

clean:
	@rm $(objects) $(program) 2> /dev/null

depend:
#	@$(DEPEND) $(INC) $(src)
