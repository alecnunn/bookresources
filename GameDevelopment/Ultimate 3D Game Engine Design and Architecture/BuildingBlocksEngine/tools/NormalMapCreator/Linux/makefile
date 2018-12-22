CC = g++ -Wall -pedantic -ansi

all:
	$(CC) ../../../LinuxObjects/Vector3D.o ../../../LinuxObjects/TGA.o ../../../LinuxObjects/Endian.o ../../../LinuxObjects/Archive.o ../../../LinuxObjects/File.o ../main.cpp -o NormalMapCreator -I /usr/include -I ../../../source

clean:
	@echo Cleaning up...
	@rm NormalMapCreator
	@echo Done.
