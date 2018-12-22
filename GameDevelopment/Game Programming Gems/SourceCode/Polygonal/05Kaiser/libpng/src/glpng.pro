TARGET		= glpng
TEMPLATE	= lib
CONFIG		= staticlib warn_on release
INCLUDEPATH	= ../include 
#DEFINES		= SGI

HEADERS		= png/png.h \
		  png/pngconf.h \
		  zlib/infblock.h \
		  zlib/infcodes.h \
		  zlib/inffast.h \
		  zlib/inffixed.h \
		  zlib/inftrees.h \
		  zlib/infutil.h \
		  zlib/zconf.h \
		  zlib/zlib.h \
		  zlib/zutil.h
SOURCES		= glpng.c \
		  png/png.c \
		  png/pngerror.c \
		  png/pngget.c \
		  png/pngmem.c \
		  png/pngpread.c \
		  png/pngread.c \
		  png/pngrio.c \
		  png/pngrtran.c \
		  png/pngrutil.c \
		  png/pngset.c \
		  png/pngtrans.c \
		  zlib/adler32.c \
		  zlib/crc32.c \
		  zlib/infblock.c \
		  zlib/infcodes.c \
		  zlib/inffast.c \
		  zlib/inflate.c \
		  zlib/inftrees.c \
		  zlib/infutil.c \
		  zlib/zutil.c
