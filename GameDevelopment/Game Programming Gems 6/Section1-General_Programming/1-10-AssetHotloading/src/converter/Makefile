COMPILER = g++
INC = 
CCFLAGS = -g -Wall $(INC)
LINKER = g++
LINKFLAGS += 
LINKLIBS +=  

DEPEND = makedepend
.SUFFIXES: .o .cpp

program = ../bin/converter
src = converter.cpp tgahelpers.cpp


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
