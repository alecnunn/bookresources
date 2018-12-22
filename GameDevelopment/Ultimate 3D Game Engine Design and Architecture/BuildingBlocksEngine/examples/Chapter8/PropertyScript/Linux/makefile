CC = g++ -Wall -pedantic -ansi

all:
	$(CC) ../../../../LinuxObjects/TokenStream.o ../../../../LinuxObjects/PropertyScript.o ../../../../LinuxObjects/File.o ../main.cpp -o demo -I /usr/include -I ../../../../source

clean:
	@echo Cleaning up...
	@rm demo
	@echo Done.
