SRC_LIST    =$(shell find ../ -maxdepth 1 -name "*.cpp") $(shell find ../linux -name "*.cpp")
OBJ_LIST    =$(notdir $(addsuffix .o, $(basename $(SRC_LIST))))
CPP         =g++
INCLUDES    =../../ ../linux
CPPFLAGS    = -g -DLINUX $(addprefix -I,$(INCLUDES))
LINK        =ld
LINK_FLAGS  =-r
LIBNAME     =Network

all: lib$(LIBNAME).a

clean:
	rm -f *.o
	rm -f lib$(LIBNAME).a

$(OBJ_LIST): $(SRC_LIST)
	$(CPP) $(CPPFLAGS) -o $@ -c $(filter %/$(notdir $(basename $@)).cpp,$(SRC_LIST)) 

lib$(LIBNAME).a: $(OBJ_LIST)
	$(LINK) $(LINK_FLAGS) -o lib$(LIBNAME).a $(OBJ_LIST)