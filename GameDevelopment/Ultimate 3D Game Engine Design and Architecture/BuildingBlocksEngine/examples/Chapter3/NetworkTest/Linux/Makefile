CC = g++ -Wall -ansi

all:
	$(CC) ../main.cpp ../../../../LinuxObjects/Endian.o ../../../../LinuxObjects/Socket.o ../../../../LinuxObjects/Network.o ../../../../LinuxObjects/File.o -o demo -I /usr/include -I ../../../../source


clean:
	@echo Cleaning up...
	@rm demo
	@echo Done.
