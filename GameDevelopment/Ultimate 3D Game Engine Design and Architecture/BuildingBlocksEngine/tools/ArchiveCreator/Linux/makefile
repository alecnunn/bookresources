CC = g++ -Wall -pedantic -ansi

all:
	$(CC) ../../../LinuxObjects/Endian.o ../../../LinuxObjects/Archive.o ../../../LinuxObjects/File.o ../main.cpp -o ArchiveCreator -I /usr/include -I ../../../source

clean:
	@echo Cleaning up...
	@rm ArchiveCreator
	@echo Done.
