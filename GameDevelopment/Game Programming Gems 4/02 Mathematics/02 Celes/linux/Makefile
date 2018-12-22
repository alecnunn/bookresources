# glut-opengl makefile

T = frustum_test

OBJS = \
 ../vgl/frustum.o \
 ../frustum_test.o

CPPFLAGS = -Wall -O2 -I..

LIBS = -L/usr/X11R6/lib -lglut -lGL -lGLU -lX11 -lm

$(T): $(OBJS)
	$(CC) -o $@ $(OBJS)  $(LIBS)

clean:
	rm ../*.o ../vgl/*.o

klean:
	rm ../*.o ../vgl/*.o $(T)
