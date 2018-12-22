LOADLIBES = -lGL -lglut -lMesaGLU -L/usr/X11R6/lib -lX11 \
	-lXi -lXmu

SimpleFlocking: CBoid.o CBox.o CFlock.o main.o mtxlib.o vector.o myprintf.o
	$(CC) *.o -o SimpleFlocking $(LOADLIBES)

