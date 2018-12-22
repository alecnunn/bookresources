INCS = -I../headers -I../libarray -I../ -I.

all:	librender.a

#GLUTView.o:	GLUTView.cc GLUTView.h
#	g++ -c GLUTView.cc $(INCS) $(GL_INCS)

GLScene.o:	GLScene.cc GLScene.h ../libarray/libarray.h
	$(CC) $(CFLAGS) -c GLScene.cc $(INCS) $(GL_INCS)

#GLConsole.o:	GLConsole.cc GLConsole.h
#	g++ -c GLConsole.cc $(INCS) $(GL_INCS)

HalfEdgeMesh.o:	HalfEdgeMesh.cc HalfEdgeMesh.h ../libarray/libarray.h
	$(CC) $(CFLAGS) -c HalfEdgeMesh.cc $(INCS) $(GL_INCS)

SubdivisionMesh.o: SubdivisionMesh.cc SubdivisionMesh.h ../libarray/libarray.h
	$(CC) $(CFLAGS) -c SubdivisionMesh.cc $(INCS) $(GL_INCS)

LoopMesh.o:	LoopMesh.cc LoopMesh.h ../libarray/libarray.h
	$(CC) $(CFLAGS) -c LoopMesh.cc $(INCS) $(GL_INCS)

ButterflyMesh.o:	ButterflyMesh.cc ButterflyMesh.h ../libarray/libarray.h
	$(CC) $(CFLAGS) -c ButterflyMesh.cc $(INCS) $(GL_INCS)

librender.a: GLScene.o HalfEdgeMesh.o SubdivisionMesh.o LoopMesh.o ButterflyMesh.o
	$(AR) r librender.a GLScene.o HalfEdgeMesh.o SubdivisionMesh.o LoopMesh.o ButterflyMesh.o

.PHONY:	clean

clean:
	$(RM) *.o
	$(RM) *.a

