INCS = -I../ -I.

all:	libarray.a

Quaternion.o:	Quaternion.c Quaternion.h libarray.h
	$(CC) $(CFLAGS) -c Quaternion.c $(INCS)

Array2D.o: Array2D.c Array2D.h libarray.h
	$(CC) $(CFLAGS) -c Array2D.c $(INCS)

Array3D.o: Array3D.c Array3D.h libarray.h
	$(CC) $(CFLAGS) -c Array3D.c $(INCS)

Array4D.o: Array4D.c Array4D.h libarray.h
	$(CC) $(CFLAGS) -c Array4D.c $(INCS)

Matrix3x3.o:	Matrix3x3.c Matrix3x3.h libarray.h
	$(CC) $(CFLAGS) -c Matrix3x3.c $(INCS)

Matrix4x4.o:	Matrix4x4.c Matrix4x4.h libarray.h
	$(CC) $(CFLAGS) -c Matrix4x4.c $(INCS)

Util.o:	Util.c Util.h libarray.h
	g++ -c Util.c $(INCS)

libarray.a: Array2D.o Array3D.o Array4D.o Matrix3x3.o Matrix4x4.o Quaternion.o Util.o
	$(AR) r libarray.a Array2D.o Array3D.o Array4D.o
	$(AR) r libarray.a Matrix3x3.o Matrix4x4.o
	$(AR) r libarray.a Quaternion.o
	$(AR) r libarray.a Util.o

.PHONY:	clean

clean:
	$(RM) *.o
	$(RM) *.a