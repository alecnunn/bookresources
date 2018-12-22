CC=g++
CFLAGS=-I../API -I../libs/FileIO/src
SRC_DIR=./
LIB_DIR=../libs
OUT_DIR=../../bin
OUT_FILE=$(OUT_DIR)/FakeGame.exe
DEFINES=-D_LINUX_

all:
	# Compiling...
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)CommandLineParser.cpp -o CommandLineParser.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)FakeGame.cpp -o FakeGame.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)ResourceCache.cpp -o ResourceCache.o
	$(CC) $(CFLAGS) $(DEFINES) -c $(SRC_DIR)ResourceParser.cpp -o ResourceParser.o
	
	# Creating the executable...
	$(CC) $(CFLAGS) -L$(LIB_DIR) -o $(OUT_FILE) FakeGame.o CommandLineParser.o ResourceCache.o ResourceParser.o $(LIB_DIR)/LIB_fileio.a
	
clean:
	rm *.o
