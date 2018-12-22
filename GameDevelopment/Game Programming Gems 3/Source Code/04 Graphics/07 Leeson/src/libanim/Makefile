INCS = -I../headers -I../libarray -I../ -I.

all:	libanim.a

Joint.o:	Joint.cc Joint.h ../libarray/libarray.h
	$(CC) $(CFLAGS) -c Joint.cc $(INCS) 

libanim.a: Joint.o
	$(AR) r libanim.a Joint.o

.PHONY:	clean

clean:
	$(RM) *.o
	$(RM) *.a