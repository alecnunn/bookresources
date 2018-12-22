SRC_LIST    =$(shell find ../ -name "*.cpp")
OBJ_LIST    =$(notdir $(addsuffix .o, $(basename $(SRC_LIST))))
INCLUDES    =../../
CPP         =g++
CPPFLAGS    = -g $(addprefix -I,$(INCLUDES)) 
LINK        =ld
LINK_FLAGS  =-r
LIBNAME     =Dispatch

all: lib$(LIBNAME).a

clean:
	rm -f *.o
	rm -f lib$(LIBNAME).a

$(OBJ_LIST): $(SRC_LIST)
	$(CPP) $(CPPFLAGS) -o $@ -c $(filter %/$(notdir $(basename $@)).cpp,$(SRC_LIST)) 

lib$(LIBNAME).a: $(OBJ_LIST)
	$(LINK) $(LINK_FLAGS) -o lib$(LIBNAME).a $(OBJ_LIST)