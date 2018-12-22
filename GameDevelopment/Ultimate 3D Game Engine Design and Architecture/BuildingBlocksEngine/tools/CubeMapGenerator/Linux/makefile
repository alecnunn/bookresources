CC = g++ -Wall -pedantic -ansi

all:
	$(CC) ../../../LinuxObjects/TGA.o ../../../LinuxObjects/Endian.o ../../../LinuxObjects/Archive.o ../../../LinuxObjects/File.o ../main.cpp -o CubeMapGenerator -I /usr/include -I ../../../source

clean:
	@echo Cleaning up...
	@rm CubeMapGenerator
	@echo Done.
