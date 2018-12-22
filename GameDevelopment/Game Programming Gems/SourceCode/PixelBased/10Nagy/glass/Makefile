## Makefile to build glass example on Linux
##
## DATE:	2000-May-02 14:34:40
##
## Copyright (C) 1995 by Gabor Nagy. All rights reserved.
##

CFLAGS	= -O3 -w -Wunused -Wformat -DUSEOpenGL -I../E3dlib/include
#CFLAGS	= -O3 -mpentiumpro -w -Wunused -Wformat  -DUSEOpenGL -Iinclude
LDFLAGS	= -lglut -lGLU -lGL -lXmu -lXt -lXext -lX11 -lXi -lm $(MK_LINK_DL) -lc
MKLF	=
LIBDIR	= /usr/X11R6/lib

INCLUDES = 	../E3dlib/include/geo.h\
		../E3dlib/include/obj.h\
		../E3dlib/include/shadow.h\
		../E3dlib/include/str.h

TARGETS	=	obj/main.o\
		obj/display.o\
		obj/geo.o\
		obj/obj.o\
		obj/polygons.o\
		obj/str.o


glass:		Makefile $(TARGETS)
		$(CC) $(MKLF) -o $@ $(TARGETS) -L$(LIBDIR) $(LDFLAGS)

clean:
		rm -f $(TARGETS) core



obj/main.o:	main.c $(INCLUDES)
		$(CC) -c $(CFLAGS) main.c -o $@

obj/display.o:	../E3dlib/src/display.c $(INCLUDES)
		$(CC) -c $(CFLAGS) ../E3dlib/src/display.c -o $@

obj/geo.o:	../E3dlib/src/geo.c $(INCLUDES)
		$(CC) -c $(CFLAGS) ../E3dlib/src/geo.c -o $@

obj/obj.o:	../E3dlib/src/obj.c $(INCLUDES)
		$(CC) -c $(CFLAGS) ../E3dlib/src/obj.c -o $@

obj/polygons.o:	../E3dlib/src/polygons.c $(INCLUDES)
		$(CC) -c $(CFLAGS) ../E3dlib/src/polygons.c -o $@

obj/str.o:	../E3dlib/src/str.c $(INCLUDES)
		$(CC) -c $(CFLAGS) ../E3dlib/src/str.c -o $@
