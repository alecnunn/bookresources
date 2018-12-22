LOADLIBES = -lGL -lglut -lGLU -L/usr/X11R6/lib -lX11 -lXi -lXmu

demolib: 3DSprite.o demolib.o gpgtext.o mtxlib.o
	$(CC) *.o -o demolib $(LOADLIBES)
