CC = g++ -Wall -pedantic -ansi

all:
	$(CC) ../../../../LinuxObjects/TokenStream.o ../../../../LinuxObjects/Plane.o ../../../../LinuxObjects/Matrix.o ../../../../LinuxObjects/Vector3D.o ../../../../LinuxObjects/File.o ../../../../LinuxObjects/Endian.o ../../../../LinuxObjects/InputLinux.o ../../../../LinuxObjects/OpenGLLinux.o ../../../../LinuxObjects/OpenGL.o ../../../../LinuxObjects/TGA.o linuxMain.cpp ../main.cpp -o demo -I /usr/include -I ../../../../source -L/usr/X11R6/lib -lGL -lGLU -lXxf86vm

clean:
	@echo Cleaning up...
	@rm demo
	@echo Done.
