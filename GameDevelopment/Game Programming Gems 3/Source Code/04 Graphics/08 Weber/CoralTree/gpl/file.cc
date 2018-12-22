/******************************************************************************

Coral Tree gpl/file.cc
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


#include "gpl/gpl.h"

/******************************************************************************
GPL_File::GPL_File (const GPL_File & file);

******************************************************************************/
GPL_File::GPL_File (const GPL_File & file)
{
	Attach (file);
}

/******************************************************************************
GPL_File::GPL_File (const char *name, const unsigned long flags);

******************************************************************************/
GPL_File::GPL_File (const char *name, const unsigned long flags)
{
	Open (name, flags);
}

/******************************************************************************
GPL_File::~GPL_File (void);

******************************************************************************/
GPL_File::~GPL_File (void)
{
	Close ();
}

/******************************************************************************
GPL_File & GPL_File::operator = (const GPL_File & file);

******************************************************************************/
GPL_File & GPL_File::operator = (const GPL_File & file)
{
	Detach();
	Attach(file);
	return *this;
}

/******************************************************************************
GPL_Boolean GPL_File::Open (const char *name, const unsigned long flags);

******************************************************************************/
GPL_Boolean GPL_File::Open (const char *name, const unsigned long flags)
{
	Close();

	/* FIRST CONFIRM THIS IS NOT A DIRECTORY */
	OSD_FileAttributes attributes;
	if(!osdFileGetAttributes((char *) name, &attributes))
	{
		if(attributes.attr & OSD_FILE_ATTR_DIR)
		{
			return FALSE;
		}
	}

	/* NOW OPEN IT */
	OSD_File file;
	if (osdFileOpen (&file, (char *) name, flags))
	{
		rep = (GPL_ReferenceRep *) new GPL_FileRep (file, name, flags);
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
GPL_Boolean GPL_File::Duplicate(const GPL_File & file);
******************************************************************************/
GPL_Boolean GPL_File::Duplicate(const GPL_File & file)
{
	if (file.IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) file.rep;
		return Open(r->name, r->flags);
		
	}
	return FALSE;
}

/******************************************************************************
GPL_Boolean GPL_File::Close(void);
******************************************************************************/
GPL_Boolean GPL_File::Close(void)
{
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		register const GPL_Boolean success = osdFileClose(&(r->file));
		Detach();
		return success;
	}
	return TRUE;
}

/******************************************************************************
GPL_Boolean GPL_File::Copy(const char *destination) const;
******************************************************************************/
GPL_Boolean GPL_File::Copy(const char *destination) const
{
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		return osdFileCopy ((char *) destination, r->name);
	}
	return FALSE;
}

/******************************************************************************
GPL_Boolean GPL_File::Delete(void);
******************************************************************************/
GPL_Boolean GPL_File::Delete(void)
{
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		register char *name = osdStrdup(r->name);
		Close ();
		register const GPL_Boolean success = osdFileDelete (name);
		osdFree (name);
		return success;
	}
	return FALSE;
}

/******************************************************************************
char *GPL_File::GetName (void) const;
******************************************************************************/
char *GPL_File::GetName (void) const
{
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		return r->name;
	}
	return (char *) 0;
}

/******************************************************************************
unsigned long GPL_File::GetSize(void) const
******************************************************************************/
unsigned long GPL_File::GetSize(void) const
{
	OSD_FileAttributes attr;
	if (!GetAttributes(attr))
	{
		return 0;
	}
	return (unsigned long) attr.size;
}

/******************************************************************************
char *GetDirectory(const char *name);
******************************************************************************/
char *GPL_File::GetDirectory(const char *name)
{
	char *buffer = OSD_GLOBAL_BUFFER[0];
	buffer[0] = 0;
	if ((char *) 0 != name)
	{
		register const size_t n = strlen(name);
		register ssize_t i;
		for (i = n - 1; 0 <= i; --i)
		{
			if (OSD_DIRECTORY_SEPARATOR == name[i])
			{
				// If the directory is the root, save the '/'.
				if (0 == i)
				{
					++i;
				}

				strncpy(buffer, name, i);
				buffer[i] = 0;
				break;
			}
		}
	}
	return buffer;
}

/******************************************************************************
char *GetBase(const char *name);
******************************************************************************/
char *GPL_File::GetBase(const char *name)
{
	char *buffer = OSD_GLOBAL_BUFFER[1];
	buffer[0] = 0;
	if ((char *) 0 != name)
	{
		register const size_t n = strlen(name);
		for (register ssize_t i = n - 1; 0 <= i; --i)
		{
			if (OSD_DIRECTORY_SEPARATOR == name[i])
			{
				strncpy(buffer, name + (1 + i), n - i);
				return buffer;
			}
		}
		strcpy(buffer, name);
	}
	return buffer;
}

/******************************************************************************
char *GetExtension(const char *name);
******************************************************************************/
char *GPL_File::GetExtension(const char *name)
{
	char *buffer = OSD_GLOBAL_BUFFER[2];
	buffer[0] = 0;
	if ((char *) 0 != name)
	{
		register const size_t n = strlen(name);
		for (register ssize_t i = n - 1; 0 <= i; --i)
		{
			if ('.' == name[i])
			{
				strncpy(buffer, name + (1 + i), n - i);
				break;
			}
		}
	}
	return buffer;
}

/******************************************************************************
char *GetFullPathName(const char *name);
******************************************************************************/
char *GPL_File::GetFullPathName(const char *name)
{
	// Convert the incoming name.
	OSD_GLOBAL_BUFFER[4][0] = 0;
	if ((char *) 0 != name)
	{
		osdPathnameConvert(OSD_GLOBAL_BUFFER[4], (char *) name);
	}

	// Ensure that it is a rooted path-name.
	if ((OSD_DIRECTORY_SEPARATOR != OSD_GLOBAL_BUFFER[4][0]) &&
		':' != OSD_GLOBAL_BUFFER[4][1])
	{
		// If the current directory can be retrieved...
		osdMemset(OSD_GLOBAL_BUFFER[3], 0, 256);
		if ((char *) 0 != osdCWDGet(OSD_GLOBAL_BUFFER[3], 256))
		{
			// Add the directory separator.
			char separator[2] = { OSD_DIRECTORY_SEPARATOR, 0 };
			strcat(OSD_GLOBAL_BUFFER[3], separator);
		}

		// Concatenate the file name.
		//!! This could fail if strlen(OSD_GLOBAL_BUFFER[3]) + strlen(name) + 1
		//!! is greater than the size of OSD_GLOBAL_BUFFER[3].
		strcat(OSD_GLOBAL_BUFFER[3], OSD_GLOBAL_BUFFER[4]);
		return OSD_GLOBAL_BUFFER[3];
	}
	return OSD_GLOBAL_BUFFER[4];
}

/******************************************************************************
unsigned long GPL_File::GetFlags (void) const;
******************************************************************************/
unsigned long GPL_File::GetFlags (void) const
{
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		return r->flags;
	}
	return 0;
}

/******************************************************************************
GPL_Boolean GPL_File::EndOfFile (void) const;
******************************************************************************/
GPL_Boolean GPL_File::EndOfFile (void) const
{
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		return r->endOfFile;
	}
	return FALSE;
}

/******************************************************************************
GPL_Boolean GPL_File::GetAttributes (const char *name, OSD_FileAttributes & attr) const;
******************************************************************************/
GPL_Boolean GPL_File::GetAttributes (const char *name, OSD_FileAttributes & attr)
{
	if ((char *) 0 != name && 0 == osdFileGetAttributes ((char *) name, &attr))
	{
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
GPL_Boolean	GetModificationTime(const char *name, OSD_Time & time);
******************************************************************************/
GPL_Boolean	GPL_File::GetModificationTime(const char *name, OSD_Time & time)
{
	OSD_FileAttributes attr;
	if ((char *) 0 != name && 0 == osdFileGetAttributes((char *) name, &attr))
	{
		time = attr.lastWrite;
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
long GPL_File::GetPointer(void)
******************************************************************************/
long GPL_File::GetPointer(void)
{
	if(IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		return osdFileSetPointer (&(r->file), 0, OSD_FILE_CURRENT);
	}
	return -1;
}

/******************************************************************************
GPL_Boolean GPL_File::SetPointer(const long offset, const long start);
******************************************************************************/
GPL_Boolean GPL_File::SetPointer (const long offset, const long start)
{
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		r->endOfFile = FALSE;
		return osdFileSetPointer (&(r->file), offset, start);
	}
	return FALSE;
}

/******************************************************************************
long GPL_File::Read (char *buffer, const unsigned long len);
******************************************************************************/
long GPL_File::Read (char *buffer, const unsigned long len)
{
	register long n = -1;
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		r->endOfFile = (0 == (n = osdFileRead (&(r->file), buffer, len)));
	}
	return n;
}

/******************************************************************************

******************************************************************************/
#define GPL_FILE_RBUFLEN 65536 
long GPL_File::Read(GPL_CharPtr &cptr)
{
	register long total = 0;
	register long n = -1;
	char buffer[GPL_FILE_RBUFLEN];
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		while(!r->endOfFile)
		{
			r->endOfFile = (0 == (n = osdFileRead (&(r->file), buffer, GPL_FILE_RBUFLEN)));
			if(n > 0)
			{
				total += n;
				cptr.InsertRange(buffer, n, cptr.GetCount());
			}
		}
	}
	if(-1 == n) return -1;
	else return total;
}

/******************************************************************************
The default here of 4096 was tested to be the optimal size on:
SunOS 5.5.1 Generic_103640-08 sun4u sparc SUNW,Ultra-1
******************************************************************************/
char *GPL_File::Read(unsigned long *length, unsigned long grain_size)
{
	register long n = -1;
	register unsigned long allocated = 0;
	char *buffer = NULL;
	if(grain_size == 0) grain_size = 4096;
	*length = 0;
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		while(!r->endOfFile)
		{
			if(allocated < *length + grain_size)
			buffer = (char *)osdRealloc(buffer, *length + grain_size);
			if(!buffer)
			{
				gplLog("GPL ERROR", "out of memory");
				return NULL;
			}
			allocated = *length + grain_size;
			r->endOfFile = (0 == (n = osdFileRead (&(r->file), &buffer[*length], grain_size)));
			if(n > 0)
			{
				*length += n;
			}
		}
	}
	if(-1 == n) 
	{
		if(buffer) osdFree(buffer);
		return NULL;
	}
	else 
	{
		return buffer;
	}
}


/******************************************************************************
long	GPL_File::ReadLine(char *buffer, const unsigned long len)

	byte-by-byte: not too efficient

	skips over \newline

******************************************************************************/
long GPL_File::ReadLine(char *buffer, const unsigned long maxlen)
{
	register long rc = -1;
	register long n = 0;

	if (IsValid())
	{
		// Read everything up to and including newline.
		register long meta = FALSE;
		register GPL_FileRep *r;
		r = (GPL_FileRep *) rep;
		while (n < (long)maxlen &&
//				1 == (rc = osdFileRead(&(r->file), &(buffer[n]), 1)) &&
				1 == (rc = Read(&(buffer[n]),1)) &&
				(meta || '\n' != buffer[n]))
		{
			if (meta)
			{
				meta = FALSE;
			}
			else if ('\\' == buffer[n])
			{
				meta = TRUE;
			}

			if ('\n' != buffer[n])
			{
				n++;
			}
			else
			{
				n--;
			}
		}

		// Return -1 on error or at the end-of-file.
		if (1 != rc)
		{
			return -1;
		}

		// Don't return with newline.
		if(0 < n && '\n' == buffer[n - 1])
		{
			n--;
		}

		// NULL terminate.
		buffer[n] = 0;

		return n;
	}

	return rc;
}

/******************************************************************************
long GPL_File::Write (char *buffer, const unsigned long len);
******************************************************************************/
long GPL_File::Write (char *buffer, const unsigned long len)
{
	return Write((const char *)buffer, len);
}

long GPL_File::Write (const char *buffer, const unsigned long len)
{
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		return osdFileWrite (&(r->file), (char *) buffer, len);
	}
	return -1;
}

/******************************************************************************
long GPL_File::Printf(char *format, ...)
******************************************************************************/
long GPL_File::Printf(char *format, ...)
{
	va_list args;
	va_start(args, format);
	char *s = osdVSPrintf(format, args);
	va_end(args);
	if ((char *) GPL_BAD_ADDRESS == s)
	{
		return -1;
	}
	register const long rc = Write((const char *) s, strlen(s));
	osdFree(s);
	return rc;
}

/******************************************************************************
GPL_Boolean Lock(long offset, long len, GPL_Boolean ex, GPL_Boolean wait);
******************************************************************************/
GPL_Boolean GPL_File::Lock(long offset, long len, GPL_Boolean ex, GPL_Boolean wait)
{
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		return osdFileLock (&(r->file), offset, len, ex, wait);
	}
	return FALSE;
}

/******************************************************************************
GPL_Boolean Unlock(long offset, long len);
******************************************************************************/
GPL_Boolean GPL_File::Unlock(long offset, long len)
{
	if (IsValid())
	{
		register GPL_FileRep *r = (GPL_FileRep *) rep;
		return osdFileUnlock (&(r->file), offset, len);
	}
	return FALSE;
}

/******************************************************************************
GPL_Boolean Exists(const char * name);
******************************************************************************/
GPL_Boolean GPL_File::Exists(const char * name)
{
	if ((char *) GPL_BAD_ADDRESS != name && osdFileExists((char *) name))
	{
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
GPL_Boolean GPL_File::Copy(const char *name, const char *copy)
******************************************************************************/
GPL_Boolean GPL_File::Copy(const char *name, const char *copy)
{
	if ((char *) GPL_BAD_ADDRESS != name &&
		(char *) GPL_BAD_ADDRESS != copy &&
		osdFileCopy((char *) copy, (char *) name))
	{
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
GPL_Boolean GPL_File::Delete(const char *name);
******************************************************************************/
GPL_Boolean GPL_File::Delete(const char *name)
{
	if ((char *) GPL_BAD_ADDRESS != name && osdFileDelete ((char *) name))
	{
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************
GPL_FileIterator::GPL_FileIterator(void)

******************************************************************************/
GPL_FileIterator::GPL_FileIterator(const char d)
	: file((GPL_File *) 0), front(0), back(0)
{
	SetDelimiter(d);
	size = GPL_FILEITERATOR_CHUNK_SIZE;
	buffer = (char *) osdMalloc(size);
	osdMemset(buffer, 0, size);
}

/******************************************************************************
GPL_FileIterator::~GPL_FileIterator(void)

******************************************************************************/
GPL_FileIterator::~GPL_FileIterator(void)
{
	if ((char *) 0 != buffer)
	{
		osdFree(buffer);
	}
}

/******************************************************************************
void GPL_FileIterator::Reset(GPL_File *f)

******************************************************************************/
void GPL_FileIterator::Reset(GPL_File *f)
{
	file = f;
	osdMemset(buffer, 0, size);
	front = 0;
	back = 0;
}

/******************************************************************************
char *GPL_FileIterator::Next(void)

******************************************************************************/
char *GPL_FileIterator::Next(void)
{
	if ((GPL_File *) 0 == file || !file->IsValid())
	{
		return (char *) 0;
	}

	// Read until the buffer contains the delimiter or
	// the end of file is reached.
	register char *del = (char *) 0;
	register unsigned long n = 0;
	while ((char *) 0 == (del = strchr(buffer + front, delimiter))
			&& !file->EndOfFile())
	{
		// Slide the bytes down.
		if (0 < front && front < back)
		{
			osdMemmove(buffer, (buffer + front), (back - front));
			back -= front;
			front = 0;
		}

		// If the buffer is full...
		if (size <= (1 + back))
		{
			size += GPL_FILEITERATOR_CHUNK_SIZE;
			buffer = (char *) osdRealloc(buffer, size);
			osdMemset((buffer + back), 0, (size - back));
		}

		// If the read fails...
		if (1 > (n = file->Read(buffer + back, size - (1 + back))))
		{
			break;
		}
		back += n;
	}

	// Mark the end of the line.
	register char *line = (front < back) ? buffer + front : (char *) 0;
	if ((char *) 0 != del)
	{
		*del = 0;
		front = 1 + del - buffer;
	}
	return line;
}

