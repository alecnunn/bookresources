CC = gcc
CXX = g++

INCLUDE = -I/usr/local/include -I/usr/X11R6/include 
LIBDIR  = -L/usr/local/lib -L/usr/X11R6/lib -L. 

WARNFLAGS=-Wall -Werror -ansi -pedantic -pedantic-errors -Wno-unused
COMPILERFLAGS = $(WARNFLAGS) -fPIC -DPIC -g -O2 -DLINUX

CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
CXXFLAGS = $(COMPILERFLAGS) $(INCLUDE)

LIBRARIES = -lopenal -lglut -lGL -lGLU -lXmu -lXext -lXi -lX11 -lpthread -ldl
COMMONOBJS = ../COMMON/audioenv.o ../COMMON/common.o ../COMMON/object.o ../COMMON/textout.o ../COMMON/Obstruct.o

.SUFFIXES: .cpp .c .o
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

all : xldemo

xldemo : $(COMMONOBJS) xlmain.o 
	$(CXX) $(CXXFLAGS) -o xldemo $(LIBDIR) xlmain.o ../COMMON/audioenv.o ../COMMON/common.o ../COMMON/object.o ../COMMON/textout.o ../COMMON/Obstruct.o $(LIBRARIES)  

clean:
	-rm -f $(COMMONOBJS) xldemo xlmain.o
