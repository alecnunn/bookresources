CC = g++ -Wall -pedantic -ansi

all:
	$(CC) ../../../../LinuxObjects/InputLinux.o ../main.cpp -o demo -I /usr/include -I ../../../../source -L/usr/X11R6/lib -lalut -lGL -lGLU -lXxf86vm

clean:
	@echo Cleaning up...
	@rm demo
	@echo Done.
