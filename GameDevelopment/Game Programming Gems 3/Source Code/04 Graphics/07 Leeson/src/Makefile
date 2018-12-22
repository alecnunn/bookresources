CC = c++
MAKE = make
CFLAGS = -g -Wall -O2
AR = ar
RM = del
GL_INCS =
# -I/usr/X11R6/include
GL_LIBS = -lglut32 -lopengl32 -lglu32
#-L/usr/X11R6/lib -lGLU -lGL -lglut
#-lXi -lXext -lX11 -lm 
#-lGL -lGLU -lglut
INCS = -Ilibrender/ -Iheaders/ -Ilibanim -I.
LIBS =  -L./librender -L./libanim -L./libarray -lrender -lanim -larray -lm
export CC MAKE CFLAGS AR RM GL_INCS GL_LIBS
all:	loop butterfly linear

loop:	main.cc main.h libarray librender libanim
	$(CC) -o loop $(CFLAGS) -D_LOOP_ main.cc  $(INCS) $(GL_INCS) $(LIBS) $(GL_LIBS)

butterfly:	main.cc main.h libarray librender libanim
	$(CC) -o butterfly $(CFLAGS) -D_BUTTERFLY_ main.cc $(INCS) $(GL_INCS) $(LIBS) $(GL_LIBS)

linear:	main.cc main.h librender libarray libanim
	$(CC) -o linear $(CFLAGS) -D_LINEAR_ main.cc $(INCS) $(GL_INCS) $(LIBS) $(GL_LIBS)

.PHONY: librender libarray libanim

librender: libarray
	$(MAKE) -C librender all

libarray:
	$(MAKE) -C libarray all

libanim: libarray
	$(MAKE) -C libanim all

clean:
	$(MAKE) -C librender clean
	$(MAKE) -C libarray clean
	$(MAKE) -C libanim clean


