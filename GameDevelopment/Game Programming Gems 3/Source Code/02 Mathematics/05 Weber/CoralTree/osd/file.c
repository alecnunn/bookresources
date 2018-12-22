
/******************************************************************************

Coral Tree osd/file.c
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
#include "osd.h"



/* have osdFileGetAttributes complain about non-existant files */
#define	OSD_FILE_ATTRIB_NONEXIST_COMPLAIN	FALSE



/******************************************************************************
long osdFileOpen(OSD_File *file, char *filename, long flags)
keywords: c, blackbox_system, file, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileOpen(file, filename, flags)
	OSD_File *file; 
	char *filename;
	long flags;
#else
long osdFileOpen(OSD_File *file, char *filename, long flags)
#endif
{
#if OSD_OSTYPE==OSD_WIN32
	DWORD access = 0;
	DWORD erase = 0;
	DWORD attr = FILE_ATTRIBUTE_NORMAL;
	file->MapHandle = NULL;
	if(flags & OSD_FILE_ERASE)
	{
		erase = CREATE_ALWAYS;
	}
	else
	{
		erase = OPEN_ALWAYS;
	}
	if(flags & OSD_FILE_READ)
	{
		access = access | GENERIC_READ;
	}
	if(flags & OSD_FILE_WRITE)
	{
		access = access | GENERIC_WRITE;
	}
	if(flags & OSD_FILE_SYNCH)
	{
		attr = attr | FILE_FLAG_WRITE_THROUGH;
	}
	if((flags & OSD_FILE_READ)&&(flags & OSD_FILE_WRITE)) 
	{
		file->MapAccess = PAGE_READWRITE;
	}
	else if(flags & OSD_FILE_READ)
	{
		file->MapAccess = PAGE_READONLY;
	}
	else
	{
		file->MapAccess = 0;
	}
	file->FileHandle = CreateFile(
		filename,
		access,
		0,
		NULL,
		erase,
		attr,
		NULL);
	if(file->FileHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
#else
	int access = 0;
	int erase = 0;
	int create = 0;
	int attr = 0;
	if(flags & OSD_FILE_ERASE)
	{
		erase = O_TRUNC;
	}
	if((flags & OSD_FILE_READ) && (flags & OSD_FILE_WRITE))
	{
		access = O_RDWR;
	}
	else if(flags & OSD_FILE_READ)
	{
		access = O_RDONLY;
	}
	else if(flags & OSD_FILE_WRITE)
	{
		access = O_WRONLY;
	}
	if((flags & OSD_FILE_WRITE) || (flags & OSD_FILE_ERASE))
	{
		create = O_CREAT;
	}
	if(flags & OSD_FILE_SYNCH)
	{
		attr = O_SYNC;
	}
	file->FileHandle = open(filename, access | erase | create | attr, 0666);
	if(file->FileHandle == -1)
	{
		return FALSE;
	}
#endif

	/* to beginning for read, to end for write (append) */
	if(!osdFileSetPointer(file, 0, (flags & OSD_FILE_WRITE)? OSD_FILE_END: OSD_FILE_BEGIN ))
		return FALSE;

	return TRUE;
}

/******************************************************************************
long osdFileClose(OSD_File *file)
keywords: c, blackbox_system, file, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileClose(file)
	OSD_File *file;
#else
long osdFileClose(OSD_File *file)
#endif
{
#if OSD_OSTYPE==OSD_WIN32
	if(file->MapHandle) CloseHandle(file->MapHandle);
	if(!CloseHandle(file->FileHandle))
	{
		return FALSE;
	}
#else
	if(close(file->FileHandle) == -1)
	{
		return FALSE;
	}
#endif
	return TRUE;
}

/******************************************************************************
long osdFileRead(OSD_File *file, char *buffer,unsigned long len)
keywords: c, blackbox_system, file, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileRead(file, buffer, len)
	OSD_File *file; 
	char *buffer;
	unsigned long len;
#else
long osdFileRead(OSD_File *file, char *buffer,unsigned long len)
#endif
{
#if OSD_OSTYPE==OSD_WIN32
	DWORD bytes_read;
	if(ReadFile(file->FileHandle,buffer,len,&bytes_read,NULL))
	{
		return bytes_read;
	}
	else
	{
		return -1;
	}
#else
	int bytes_read;
	if((bytes_read=read(file->FileHandle,buffer,(unsigned int)len)) != -1)
	{
		return bytes_read;
	}
	else
	{
		return -1;
	}
#endif
}

/******************************************************************************
long osdFileWrite(OSD_File *file, char *buffer,unsigned long len)
keywords: c, blackbox_system, file, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileWrite(file, buffer, len)
	OSD_File *file; 
	const char *buffer;
	unsigned long len;
#else
long osdFileWrite(OSD_File *file, const char *buffer,unsigned long len)
#endif
{
#if OSD_OSTYPE==OSD_WIN32
	DWORD bytes_written;
	if(WriteFile(file->FileHandle,buffer,len,&bytes_written,NULL))
	{
		return bytes_written;
	}
	else
	{
		return -1;
	}
#else
	int bytes_written;
	if((bytes_written=write(file->FileHandle,buffer,(unsigned int)len)) != -1)
	{
		return bytes_written;
	}
	else
	{
		return -1;
	}
#endif
}

/******************************************************************************
long osdFileSetPointer(OSD_File *file, long offset, long start)
keywords: c, blackbox_system, file, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileSetPointer(file, offset, start)
	OSD_File *file; 
	long offset; 
	long start;
#else
long osdFileSetPointer(OSD_File *file, long offset, long start)
#endif
{
	if(offset < 0)
	{
		osdPrint(OSD_SOFTWARE,OSD_ERROR,"negative offset not supported");
		return FALSE;
	}
	switch(start)
	{
		case OSD_FILE_BEGIN:
#if OSD_OSTYPE==OSD_WIN32
				SetFilePointer(file->FileHandle,offset,NULL,FILE_BEGIN);
#else
				lseek(file->FileHandle,offset,SEEK_SET);
#endif
			break;
		case OSD_FILE_CURRENT:
#if OSD_OSTYPE==OSD_WIN32
				SetFilePointer(file->FileHandle,offset,NULL,FILE_CURRENT);
#else
				lseek(file->FileHandle,offset,SEEK_CUR);
#endif
			break;
		case OSD_FILE_END:
#if OSD_OSTYPE==OSD_WIN32
				SetFilePointer(file->FileHandle,offset,NULL,FILE_END);
#else
				lseek(file->FileHandle,offset,SEEK_END);
#endif
			break;
	}
	return TRUE;
}


/******************************************************************************
long osdFileLock(OSD_File *file, long offset, long len)
keywords: c, blackbox_system, file, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileLock(file, offset, len, ex, wait)
	OSD_File *file; 
	long offset; 
	long len;
	long ex;
	long wait;
#else
long osdFileLock(OSD_File *file, long offset, long len, long ex, long wait)
#endif
{
	long flags;
#if OSD_OSTYPE==OSD_WIN32
	OVERLAPPED overlapped;
#else /* OSD_OSTYPE!=OSD_WIN32 */
	struct flock lock;
#endif /* OSD_OSTYPE==OSD_WIN32 */

	if(offset < 0)
	{
		osdPrint(OSD_SOFTWARE,OSD_ERROR,"negative offset not supported");
		return FALSE;
	}

#if OSD_OSTYPE==OSD_WIN32
	flags = 0;
	if (ex)
	{
		flags = flags | LOCKFILE_EXCLUSIVE_LOCK;
		flags = flags & ~LOCKFILE_FAIL_IMMEDIATELY;
	}
	if (wait)
	{
		flags = flags & ~LOCKFILE_EXCLUSIVE_LOCK;
		flags = flags | LOCKFILE_FAIL_IMMEDIATELY;
	}

	overlapped.Internal = 0;
	overlapped.InternalHigh = 0;
	overlapped.Offset = offset;
	overlapped.OffsetHigh = 0;
	overlapped.hEvent = OSD_INVALID_HANDLE;

	if(!osdIsWin95())
	{
		return LockFileEx(file->FileHandle, flags, 0, len, 0, &overlapped);
	}
	else if(flags & LOCKFILE_EXCLUSIVE_LOCK)
	{
		return LockFile(file->FileHandle, offset, 0, len, 0);
	}
	else
	{
		return TRUE;
	}
#else /* OSD_OSTYPE!=OSD_WIN32 */
	if (ex)
	{
		lock.l_type = F_WRLCK;
	}
	else
	{
		lock.l_type = F_RDLCK;
	}
	lock.l_start = offset;
	lock.l_whence = SEEK_SET;
	lock.l_len = len;
	lock.l_pid = 0;
	if (wait)
	{
		flags = F_SETLKW;
	}
	else
	{
		flags = F_SETLK;
	}

	return -1 != fcntl(file->FileHandle, flags, &lock);
#endif /* OSD_OSTYPE==OSD_WIN32 */
}


/******************************************************************************
long osdFileUnlock(OSD_File *file, long offset, long len)
keywords: c, blackbox_system, file, system

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileUnlock(file, offset, len)
	OSD_File *file; 
	long offset; 
	long len;
#else
long osdFileUnlock(OSD_File *file, long offset, long len)
#endif
{
#if OSD_OSTYPE==OSD_WIN32
	OVERLAPPED overlapped;
#else /* OSD_OSTYPE!=OSD_WIN32 */
	struct flock lock;
#endif /* OSD_OSTYPE==OSD_WIN32 */

	if(offset < 0)
	{
		osdPrint(OSD_SOFTWARE,OSD_ERROR,"negative offset not supported");
		return FALSE;
	}

#if OSD_OSTYPE==OSD_WIN32
	overlapped.Internal = 0;
	overlapped.InternalHigh = 0;
	overlapped.Offset = offset;
	overlapped.OffsetHigh = 0;
	overlapped.hEvent = OSD_INVALID_HANDLE;
	if(!osdIsWin95())
	{
		return UnlockFileEx(file->FileHandle, 0, len, 0, &overlapped);
	}
	else
	{
		return UnlockFile(file->FileHandle, offset, 0, len, 0);
	}
#else /* OSD_OSTYPE!=OSD_WIN32 */
	lock.l_type = F_UNLCK;
	lock.l_start = offset;
	lock.l_whence = SEEK_SET;
	lock.l_len = len;
	lock.l_pid = 0;

	/* No need to wait for an unlock, hence F_SETLK. */
	return -1 != fcntl(file->FileHandle, F_SETLK, &lock);
#endif /* OSD_OSTYPE==OSD_WIN32 */
}


/******************************************************************************
long	osdFileGetAttributes(char *filename,OSD_FileAttributes *attributes)

	returns FALSE if successful

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileGetAttributes(filename,attributes)
	char *filename;
	OSD_FileAttributes *attributes;
#else
long osdFileGetAttributes(char *filename,OSD_FileAttributes *attributes)
#endif
	{

	long is_link=FALSE,is_dir=FALSE;
	long read=TRUE,write=TRUE,execute=FALSE;
	long size=0;
	long fail=FALSE;
	OSD_TIME lastWrite;
	
#if OSD_OSTYPE==OSD_UNIX
	char comment[100];

	if( !(access(filename,F_OK)==0) )
		{
#if OSD_FILE_ATTRIB_NONEXIST_COMPLAIN
		sprintf(comment,"\"%s\" doesn't exist\n",filename);
		osdPrint(OSD_SOFTWARE,OSD_ERROR,comment);
#endif
		fail=TRUE;
		}
	else
		{

		struct stat statdata;

		read=   (access(filename,R_OK)==0);
		write=  (access(filename,W_OK)==0);
		execute=(access(filename,X_OK)==0);

		if( lstat(filename,&statdata) )
			{
			sprintf(comment,"couldn't lstat() \"%s\"\n",filename);
			osdPrint(OSD_SOFTWARE,OSD_ERROR,comment);
			fail=TRUE;
			}
		else
			{
			/* check if a link */
			if( (statdata.st_mode & S_IFLNK) == S_IFLNK)
				{
				is_link=TRUE;
				fail=stat(filename,&statdata);
				}

			if(fail)
				{
				sprintf(comment,"couldn't stat() \"%s\"\n",filename);
				osdPrint(OSD_SOFTWARE,OSD_ERROR,comment);
				}
			else
				{
	/* 			is_dir= (statdata.st_nlink>1); */
				is_dir= (statdata.st_mode & S_IFDIR) == S_IFDIR;
				size=statdata.st_size;
				lastWrite=statdata.st_mtime;
				}
			}
		}

#elif OSD_OSTYPE==OSD_WIN32

	HANDLE filehandle;
	WIN32_FIND_DATA finddata;

	long length=strlen(filename);
	long word;

	char *suffix=&filename[length-3];
	char modname[128];

	if(filename[0]=='/')
		strcpy(modname,OSD_FILE_DEFAULT_DRIVE);
	else
		modname[0]=0;

	strcat(modname,filename);

	filehandle=FindFirstFile(modname,&finddata);

	word=finddata.dwFileAttributes;

	if(filehandle==INVALID_HANDLE_VALUE)
		{
#if OSD_FILE_ATTRIB_NONEXIST_COMPLAIN
		sprintf(comment,"FindFirstFile() couldn't match \"%s\"",modname);
		osdPrint(OSD_SOFTWARE,OSD_ERROR,comment);
#endif
		fail=TRUE;
		}

	if(!fail && FindClose(filehandle) != TRUE)
		{
		osdPrint(OSD_SOFTWARE,OSD_ERROR,"FindClose() couldn't close handle on file match");
		fail=TRUE;
		}

	if(!fail)
		{
		size=finddata.nFileSizeLow;

		is_dir= (word&FILE_ATTRIBUTE_DIRECTORY) != 0;
		write=  !(word&FILE_ATTRIBUTE_READONLY);

		execute=(	is_dir ||
					!strcmp(suffix,"exe") || !strcmp(suffix,"EXE") ||
					!strcmp(suffix,"bat") || !strcmp(suffix,"BAT") ||
					!strcmp(suffix,"com") || !strcmp(suffix,"COM") );
		FileTimeToSystemTime(&(finddata.ftLastWriteTime), &lastWrite);
		}

#else
	#error OSD_OSTYPE not properly defined
#endif

	attributes->access= read*OSD_FILE_ATTR_READ + write*OSD_FILE_ATTR_WRITE + execute*OSD_FILE_ATTR_EXE;
	attributes->attr= is_dir*OSD_FILE_ATTR_DIR + is_link*OSD_FILE_ATTR_LINK;
	attributes->size=size;
	osdMemset(&(attributes->lastWrite), 0, sizeof(attributes->lastWrite));
	osdTimeConvert(&(attributes->lastWrite), &lastWrite);

	sprintf(attributes->string,"%c%c%c%c%c %d",	is_dir?		'd': '-',
												is_link?	'l': '-',
												read?		'r': '-',
												write?		'w': '-',
												execute?	'x': '-',
												(int)size);

	return fail;
	}

/******************************************************************************
long	osdFileExists(char *filename)

	returns TRUE if filename exists

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileExists(filename)
	char *filename;
#else
long osdFileExists(char *filename)
#endif
{
#if OSD_OSTYPE==OSD_UNIX
	if (0 == access(filename, F_OK))
	{
		return TRUE;
	}
#elif OSD_OSTYPE==OSD_WIN32
	char name[OSD_MAXOUTPUTFILENAMELENGTH];

	// Build the file name.
	if ('/' == filename[0])
	{
		strcpy(name, OSD_FILE_DEFAULT_DRIVE);
	}
	else
	{
		name[0] = 0;
	}
	strcat(name, filename);

	// Get the attributes for the file.
	if (0xFFFFFFFF != GetFileAttributes (name))
	{
		return TRUE;
	}
#else
	#error OSD_OSTYPE not properly defined
#endif

	return FALSE;
}


/******************************************************************************
OSD_FILE_HANDLE osdFileGetStdHandle(long which);

	returns the file handle for a standard file

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
OSD_FILE_HANDLE osdFileGetStdHandle(which)
	long which;
#else
OSD_FILE_HANDLE osdFileGetStdHandle(long which)
#endif
{
	OSD_FILE_HANDLE handle;

#if OSD_OSTYPE==OSD_WIN32
	switch(which)
	{
	case OSD_FILE_STDIN:
		handle = GetStdHandle(STD_INPUT_HANDLE);
		break;

	case OSD_FILE_STDOUT:
		handle = GetStdHandle(STD_OUTPUT_HANDLE);
		break;

	case OSD_FILE_STDERR:
		handle = GetStdHandle(STD_ERROR_HANDLE);
		break;

	default:
		handle = OSD_INVALID_FILE_HANDLE;
		break;
	}
#elif OSD_OSTYPE==OSD_UNIX
	switch(which)
	{
	case OSD_FILE_STDIN:
		handle = STDIN_FILENO;
		break;

	case OSD_FILE_STDOUT:
		handle = STDOUT_FILENO;
		break;

	case OSD_FILE_STDERR:
		handle = STDERR_FILENO;
		break;

	default:
		handle = OSD_INVALID_FILE_HANDLE;
		break;
	}
#endif /* OSD_OSTYPE==OSD_UNIX */

	return handle;
}


/******************************************************************************
long	osdFileIsDiskFile(OSD_FILE_HANDLE handle)

	returns TRUE if handle is a file on disk, FALSE otherwise

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileIsDiskFile(handle)
	OSD_FILE_HANDLE handle;
#else
long osdFileIsDiskFile(OSD_FILE_HANDLE handle)
#endif
	{
#if OSD_OSTYPE==OSD_WIN32
	return FILE_TYPE_DISK == GetFileType(handle);
#elif OSD_OSTYPE==OSD_UNIX
	return !isatty(handle);
#else /* OSD_OSTYPE!=OSD_UNIX */
#endif /* OSD_OSTYPE==OSD_WIN32 */
	}



/******************************************************************************
long	osdFileIsTerminal(OSD_FILE_HANDLE handle)

	returns TRUE if handle is a terminal device, FALSE otehrwise

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileIsTerminal(handle)
	OSD_FILE_HANDLE handle;
#else
long osdFileIsTerminal(OSD_FILE_HANDLE handle)
#endif
	{
#if OSD_OSTYPE==OSD_WIN32
	return _isatty(handle);
#elif OSD_OSTYPE==OSD_UNIX
	return isatty(handle);
#else /* OSD_OSTYPE!=OSD_UNIX */
#endif /* OSD_OSTYPE==OSD_WIN32 */
	}



/******************************************************************************
long	osdFileCopy(char *target_path,char *source_path)

	copy a file from source_path to target_path

	returns nonzero on error

	NOTE: should change to segment the copy instead of creating a big buffer

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileCopy(target_path,source_path)
	char *target_path;
	char *source_path;
#else
long osdFileCopy(char *target_path,char *source_path)
#endif
	{
/* 	printf("osdFileCopy() %s <- %s\n",target_path,source_path); */

#if OSD_OSTYPE==OSD_UNIX

	OSD_File ofile;

	OSD_FileAttributes attributes;

	long items;

	char *buffer;

	/* read file */

	if( !osdFileOpen(&ofile,source_path,OSD_FILE_READ) )
		return -1;

	osdFileSetPointer(&ofile,0,OSD_FILE_BEGIN);

	osdFileGetAttributes(source_path,&attributes);

	buffer=(char *)osdMalloc(attributes.size);

	items=osdFileRead(&ofile,buffer,attributes.size);

	osdFileClose(&ofile);

	if(items!=attributes.size)
		{
		osdPrint(OSD_SOFTWARE,OSD_LOG,"osdFileCopy() error reading file");
		return -2;
		}


	/* write file */

	if( !osdFileOpen(&ofile,target_path,OSD_FILE_WRITE) )
		return -3;

	osdFileSetPointer(&ofile,0,OSD_FILE_BEGIN);

	items=osdFileWrite(&ofile,buffer,attributes.size);

	if(items!=attributes.size)
		{
		osdPrint(OSD_SOFTWARE,OSD_LOG,"osdFileCopy() error writing file");
		return -4;
		}

	return FALSE;

#elif OSD_OSTYPE==OSD_WIN32

	// copy using overwrite
	return !CopyFile(source_path,target_path,FALSE);

#else
	#error OSD_OSTYPE not properly defined
#endif
	}



/******************************************************************************
long	osdFileDelete(char *target_path)

	returns nonzero on error

Keywords:
******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileDelete(target_path)
	char *target_path;
#else
long osdFileDelete(char *target_path)
#endif
	{
/* 	printf("osdFileDelete() %s\n",target_path); */

#if OSD_OSTYPE==OSD_UNIX

	return unlink(target_path);

#elif OSD_OSTYPE==OSD_WIN32

	return !DeleteFile(target_path);

#else
	#error OSD_OSTYPE not properly defined
#endif
	}

/******************************************************************************

******************************************************************************/
#if OSD_COMPILER==OSD_KR
char *osdFileMap(file, len)
	OSD_File *file;
	unsigned long len;
#else
char *osdFileMap(OSD_File *file, unsigned long len)
#endif
{
#if OSD_OSTYPE==OSD_UNIX
	if((file->MapPtr = (char *)mmap(0, len, PROT_READ | PROT_WRITE, MAP_SHARED, file->FileHandle, 0)) == MAP_FAILED)
	{
		osdPrint(OSD_SYSTEM, OSD_ERROR, "could not map file");
		file->MapPtr = NULL;
	}

	file->MapLen = len;

	return file->MapPtr;
#elif OSD_OSTYPE==OSD_WIN32
	DWORD access;
	
	file->MapHandle = CreateFileMapping(
		file->FileHandle,
		NULL,
		file->MapAccess,
		0,
		len,
		NULL);

	if(!file->MapHandle)
	{
		osdPrint(OSD_SYSTEM, OSD_ERROR, "could not map file");
		return NULL;
	}
		
	if(file->MapAccess == PAGE_READWRITE) access = FILE_MAP_WRITE;
	else if(file->MapAccess == PAGE_READONLY) access = FILE_MAP_READ;

	file->MapPtr = MapViewOfFileEx(
		file->MapHandle,
		access,
		0,
		0,
		0,
		NULL);

	return (char *) file->MapPtr;
	
#else
	#error OSD_OSTYPE not properly defined
#endif
}

/******************************************************************************

******************************************************************************/
#if OSD_COMPILER==OSD_KR
long osdFileUnmap(file)
	OSD_File *file;
#else
long osdFileUnmap(OSD_File *file)
#endif
{
#if OSD_OSTYPE==OSD_UNIX
	if(!file->MapPtr) return FALSE;

	if(munmap(file->MapPtr, file->MapLen) == 0)
	{
		return TRUE;
	}
	else
	{
		osdPrint(OSD_SYSTEM,OSD_ERROR,"could not unmap file from memory");
		return FALSE;
	}
#elif OSD_OSTYPE==OSD_WIN32
	UnmapViewOfFile(file->MapPtr);
	CloseHandle(file->MapHandle);
	file->MapHandle = NULL;
	return TRUE;
#else
	#error OSD_OSTYPE not properly defined
#endif
}

