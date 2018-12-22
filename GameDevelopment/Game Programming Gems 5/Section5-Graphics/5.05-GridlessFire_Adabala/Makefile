
SDKROOT=/home/hari

MAIN=GLFireDemo

CC=g++

CFLAGS= -DUNIX  -Wall -g 

INCLUDES= -I$(SDKROOT)/SDK/DEMOS/OpenGL/inc -I$(SDKROOT)/SDK/inc

LFLAGS = -lglut -lGLU -lGL
LFLAGS += -L$(SDKROOT)/SDK/DEMOS/OpenGL/src/shared -lnvsdk 
LFLAGS += -L$(SDKROOT)/SDK/LIBS/lib -lnvutil -lnvparse
LFLAGS += -L/usr/X11R6/lib -lXmu -lX11 -lXi -lXxf86vm -ljpeg -lpng -lunzip 

SRCS =  glinterface.cpp texture.cpp pbuffer.cpp loadpgm.cpp GLFireDemo.cpp kd_tree.cpp pqueue.cpp particle_map.cpp dyn_particle_map.cpp

HDRS =  glinterface.h texture.h pbuffer.h loadpgm.h mathlib.h kd_tree.h pqueue.h particle_map.h dyn_particle_map.h

OBJS = $(SRCS:.cpp=.o)

$(MAIN): $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) $(OBJS) -o $(MAIN) $(LFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	rm -f  *.o *.tga *~ $(MAIN)
