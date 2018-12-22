LOADLIBES = -lGL -lglut -lGLU -L/usr/X11R6/lib -lX11 \
	-lXi -lXmu 

Octree: BoxSide.o List.o Octree.o Test.o TriBox.o mtxlib.o
	$(CC) *.o -o Octree $(LOADLIBES)

