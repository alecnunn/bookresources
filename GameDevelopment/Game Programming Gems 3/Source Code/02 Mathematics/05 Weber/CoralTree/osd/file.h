/******************************************************************************

Coral Tree osd/file.h
Copyright (C) 1998 Infinite Monkeys Incorporated

This program is free software; you can redistribute it and/or modify
it under the terms of the Artistic License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Artistic License for more details.

You should have received a copy of the Artistic License along with
this program (see meta/ART_LICE); if not, write to coral@imonk.com.

******************************************************************************/



#ifdef __cplusplus
    extern "C"
    {
#endif


#define OSD_FILE_OPEN			1

#define	OSD_FILE_BEGIN			1
#define OSD_FILE_CURRENT		2
#define OSD_FILE_END			3

#define OSD_EOF					-2

/* Values for osdFileGetStdHandle() */
#define	OSD_FILE_STDIN			0
#define	OSD_FILE_STDOUT			1
#define	OSD_FILE_STDERR			2

/* FLAGS FOR osdFileOpen */
#define OSD_FILE_READ			1
#define OSD_FILE_WRITE			2
#define OSD_FILE_ERASE			4
#define OSD_FILE_SYNCH			8

/* OR'ed values for OSD_FileAttributes.access */
#define OSD_FILE_ATTR_NULL  0
#define OSD_FILE_ATTR_EXE   1
#define OSD_FILE_ATTR_WRITE 2
#define OSD_FILE_ATTR_READ  4

/* OR'ed values for OSD_FileAttributes.attr */
#define OSD_FILE_ATTR_DIR   1
#define OSD_FILE_ATTR_LINK  2


#define OSD_FILE_DEFAULT_DRIVE	"c:"


typedef struct _OSD_File
{
#if OSD_OSTYPE==OSD_WIN32
	HANDLE			FileHandle;
	HANDLE			MapHandle;
	LPVOID			MapPtr;
	DWORD			MapAccess;
#else
	int				FileHandle;
	char 			*MapPtr;
	unsigned long	MapLen;
#endif
} OSD_File;

typedef struct _OSD_FileAttributes
    {
    char string[20];
    long size;
    long attr;
    long access;
	OSD_Time lastWrite;
    } OSD_FileAttributes;


#if OSD_COMPILER==OSD_KR

long osdFileOpen();
long osdFileClose();
long osdFileRead();
long osdFileWrite();
long osdFileSetPointer();
long osdFileLock();
long osdFileUnlock();
long osdFileGetAttributes();
long osdFileExists();
OSD_FILE_HANDLE osdFileGetStdHandle();
long osdFileIsDiskFile();
long osdFileIsTerminal();
long osdFileCopy();
long osdFileDelete();
char *osdFileMap(); 
long osdFileUnmap();

#else

long osdFileOpen(OSD_File *file, char *filename, long flags);
long osdFileClose(OSD_File *file);
long osdFileRead(OSD_File *file, char *buffer,unsigned long len);
long osdFileWrite(OSD_File *file, const char *buffer,unsigned long len);
long osdFileSetPointer(OSD_File *file, long offset, long start);
long osdFileLock(OSD_File *file, long offset, long len, long ex, long wait);
long osdFileUnlock(OSD_File *file, long offset, long len);
long osdFileGetAttributes(char *filename,OSD_FileAttributes *attributes);
long osdFileExists(char *filename);
OSD_FILE_HANDLE osdFileGetStdHandle(long which);
long osdFileIsDiskFile(OSD_FILE_HANDLE handle);
long osdFileIsTerminal(OSD_FILE_HANDLE handle);
long osdFileCopy(char *target_path,char *source_path);
long osdFileDelete(char *target_path);
char *osdFileMap(OSD_File *file, unsigned long len); 
long osdFileUnmap(OSD_File *file);

#endif

/* TODO: support more options such as append vs. replace a file etc. */


#ifdef __cplusplus
    }
#endif
